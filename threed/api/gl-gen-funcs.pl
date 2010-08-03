#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;

# Read the function definitions from the input.
my @functions = ();
my %func_info = ();
while (<>)
{
    my ($returnType, $name, $argstr) =
        m/^(\w+|const GLubyte\*)\s+(\w+)\s*\(([^)]*)\)/;

    if (!$returnType) {
        my ($tag, $value) = m/^\s+(\w+)\s+(.*)$/;
        next unless $tag;
        $func_info{$tag} = $value;
        $func_info{'funcname'} = $value if ($tag eq "es_name");
        next;
    }

    if ($func_info{'name'}) {
        push @functions, { %func_info };
        %func_info = ();
    }

    $argstr =~ s/^\s+//;
    $argstr =~ s/\s+$//;
    my @args = split /,\s*/,$argstr;

    my @argNames = ();
    if ($argstr ne 'void') {
        foreach (@args) {
            my ($argType, $argName) = m/^(\w+|\w+.*\*)\s+(\w+)$/;
            push @argNames, $argName;
        }
    } else {
        $argstr = "";
    }
    my $argnamestr = join(', ', @argNames);

    $func_info{'name'} = $name;
    $func_info{'funcname'} = $name;
    $name =~ s/^gl//;
    $func_info{'varname'} = lcfirst($name);
    $func_info{'returnType'} = $returnType;
    $func_info{'argstr'} = $argstr;
    $func_info{'argnamestr'} = $argnamestr;
}
if ($func_info{'name'}) {
    push @functions, { %func_info };
}

# Generate the declarations for qglfunctions.h.
print "// qglfunctions.h\n\n";
print "#ifndef Q_WS_MAC\n";
print "# ifndef QGLF_APIENTRYP\n";
print "#   ifdef QGLF_APIENTRY\n";
print "#     define QGLF_APIENTRYP QGLF_APIENTRY *\n";
print "#   else\n";
print "#     define QGLF_APIENTRY\n";
print "#     define QGLF_APIENTRYP *\n";
print "#   endif\n";
print "# endif\n";
print "#else\n";
print "# define QGLF_APIENTRY\n";
print "# define QGLF_APIENTRYP *\n";
print "#endif\n";
print "\n";
print "struct QGLFunctionsPrivate;\n";
print "\n";

print "// Undefine any macros from GLEW, qglextensions_p.h, etc that\n";
print "// may interfere with the definition of QGLFunctions.\n";
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    my $name = $_->{'funcname'};
    print "#undef $name\n";
}
print "\n";

# Output the prototypes into the QGLFunctions class.
print "class Q_QT3D_EXPORT QGLFunctions\n";
print "{\n";
print "public:\n";
print "    QGLFunctions();\n";
print "    explicit QGLFunctions(const QGLContext *context);\n";
print "    ~QGLFunctions() {}\n";
print "\n";
print "    void setContext(const QGLContext *context);\n";
print "\n";
my $last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'funcname'};
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $_->{'returnType'} $name($_->{'argstr'});\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

print "\n";
print "private:\n";
print "    QGLFunctionsPrivate *d_ptr;\n";
print "};\n";
print "\n";
print "struct QGLFunctionsPrivate\n";
print "{\n";
print "    QGLFunctionsPrivate(const QGLContext *context = 0);\n";
print "\n";
print "#ifndef QT_OPENGL_ES_2";
print "\n";

# Output the function pointers into the QGLFunctionsPrivate class.
$last_shader_only = 0;
foreach ( @functions ) {
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    next if $inline && $inline eq 'all_diff';
    my $name = $_->{'varname'};
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $_->{'returnType'} (QGLF_APIENTRYP $name)($_->{'argstr'});\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

print "#endif\n";
print "};\n";
print "\n";

my %platform_defines = ();
$platform_defines{'es1'} = "defined(QT_OPENGL_ES_1)";
$platform_defines{'es2'} = "defined(QT_OPENGL_ES_2)";
$platform_defines{'es'} = "defined(QT_OPENGL_ES)";
$platform_defines{'desktop'} = "!defined(QT_OPENGL_ES)";

# Output the inline functions that call either the raw GL function
# or resolve via the function pointer.
$last_shader_only = 0;
foreach ( @functions ) {
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $funcname = $_->{'funcname'};
    my $varname = $_->{'varname'};
    my $is_void = ($_->{'returnType'} eq 'void');
    my $inline = $_->{'inline'};
    next if ($inline && $inline eq 'all');
    #print "#ifndef QT_OPENGL_ES_1\n\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n\n" if (!$shader_only && $last_shader_only);
    print "inline $_->{'returnType'} QGLFunctions::$funcname($_->{'argstr'})\n";
    print "{\n";
    if ($_->{'es_name'}) {
        # Functions like glClearDepth() that are inline, but named differently.
        print "#ifndef QT_OPENGL_ES\n";
        print "    ::$_->{'name'}($_->{'argnamestr'});\n";
        print "#else\n";
        print "    ::$_->{'es_name'}($_->{'argnamestr'});\n";
        print "#endif\n";
    } elsif ($inline && $inline eq 'all') {
        # Inlined on all platforms.
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "::$_->{'name'}($_->{'argnamestr'});\n";
    } elsif ($inline) {
        # Inlined only on certain platforms.
        my @platforms = split /,\s*/,$inline;
        my @defines = ();
        foreach (@platforms) {
            push @defines, $platform_defines{$_};
        }
        print "#if ";
        print join(' || ', @defines);
        print "\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "::$_->{'name'}($_->{'argnamestr'});\n";
        print "#else\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "d_ptr->$varname($_->{'argnamestr'});\n";
        print "#endif\n";
    } else {
        # Resolve on all platforms.
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "d_ptr->$varname($_->{'argnamestr'});\n";
    }
    print "}\n\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;

print "// qglfunctions.cpp\n\n";

# Generate qdoc documentation for all of the functions.
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';

    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');

    my $name = $_->{'funcname'};

    my $docargs = $_->{'argnamestr'};
    if (length($docargs) > 0) {
        $docargs =~ s/,/, \\a/g;
        $docargs =~ s/^/\\a /;
    }

    my $khronos_name = $_->{'es_name'};
    if (!$khronos_name) {
        $khronos_name = $_->{'name'};
    }

    print "/*!\n";
    print "    \\fn $_->{'returnType'} QGLFunctions::$name($_->{'argstr'})\n";
    print "\n";
    if ($khronos_name eq $_->{'name'}) {
        print "    Convenience function that calls $khronos_name($docargs).\n";
    } else {
        print "    Convenience function that calls $_->{'name'}($docargs) on\n";
        print "    desktop OpenGL systems and $khronos_name($docargs) on\n";
        print "    embedded OpenGL/ES systems.\n";
    }
    print "\n";
    print "    For more information, see the OpenGL/ES 2.0 documentation for\n";
    print "    \\l{http://www.khronos.org/opengles/sdk/docs/man/$khronos_name.xml}{$khronos_name()}.\n";
    if ($shader_only) {
        print "\n";
        print "    This convenience function will do nothing on OpenGL/ES 1.x systems.\n";
    }
    print "*/\n\n";
}

# Generate the resolver functions.
print "#ifndef QT_OPENGL_ES_2\n\n";
$last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';
    next if $inline && $inline eq 'all_diff';
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'varname'};
    my $resolver_name = $_->{'name'};
    $resolver_name =~ s/^gl/qglfResolve/;
    my $special_name = $_->{'name'};
    $special_name =~ s/^gl/qglfSpecial/;
    my @platforms = split /,\s*/,$inline;
    $shader_only = 1 if @platforms ~~ 'es1';
    my $is_void = ($_->{'returnType'} eq 'void');
    my $special_handling = ($_->{'special_handling'} && $_->{'special_handling'} eq 'yes');
    #print "#ifndef QT_OPENGL_ES_1\n\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n\n" if (!$shader_only && $last_shader_only);

    if ($special_handling) {
        # Output special fallback implementations for certain functions.
        if ($name eq "blendEquationSeparate") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    const QGLContext *context = QGLContext::currentContext();\n";
            print "    QGLFunctionsPrivate *funcs = qt_gl_functions(context);\n";
            print "    Q_UNUSED(modeAlpha);\n";
            print "    funcs->blendEquation(modeRGB);\n";
            print "}\n\n";
        } elsif ($name eq "blendFuncSeparate") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(srcAlpha);\n";
            print "    Q_UNUSED(dstAlpha);\n";
            print "    ::glBlendFunc(srcRGB, dstRGB);\n";
            print "}\n\n";
        } elsif ($name eq "getShaderPrecisionFormat") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(shadertype);\n";
            print "    Q_UNUSED(precisiontype);\n";
            print "    range[0] = range[1] = precision[0] = 0;\n";
            print "}\n\n";
        } elsif ($name eq "isProgram" || $name eq "isShader") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    return $_->{'argnamestr'} != 0;\n";
            print "}\n\n";
        } elsif ($name eq "releaseShaderCompiler") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "}\n\n";
        } elsif ($name eq "shaderBinary") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(n);\n";
            print "    Q_UNUSED(shaders);\n";
            print "    Q_UNUSED(binaryformat);\n";
            print "    Q_UNUSED(binary);\n";
            print "    Q_UNUSED(length);\n";
            print "}\n\n";
        } elsif ($name eq "stencilFuncSeparate") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(face);\n";
            print "    ::glStencilFunc(func, ref, mask);\n";
            print "}\n\n";
        } elsif ($name eq "stencilMaskSeparate") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(face);\n";
            print "    ::glStencilMask(mask);\n";
            print "}\n\n";
        } elsif ($name eq "stencilOpSeparate") {
            print "static $_->{'returnType'} $special_name($_->{'argstr'})\n";
            print "{\n";
            print "    Q_UNUSED(face);\n";
            print "    ::glStencilOp(fail, zfail, zpass);\n";
            print "}\n\n";
        }
    }

    print "static $_->{'returnType'} $resolver_name($_->{'argstr'})\n";
    print "{\n";
    my $type_name = "type_$_->{'name'}";
    print "    typedef $_->{'returnType'} (QGLF_APIENTRYP $type_name)($_->{'argstr'});\n\n";
    print "    const QGLContext *context = QGLContext::currentContext();\n";
    print "    QGLFunctionsPrivate *funcs = qt_gl_functions(context);\n";
    print "\n";
    print "    funcs->$name = ($type_name)\n";
    print "        context->getProcAddress(QLatin1String(\"$_->{'name'}\"));\n";
    my @alt_names = ();
    if ($_->{'alt_name'}) {
        push @alt_names, $_->{'alt_name'};
    } else {
        push @alt_names, "$_->{'name'}OES";
        push @alt_names, "$_->{'name'}EXT";
        push @alt_names, "$_->{'name'}ARB";
    }
    foreach (@alt_names) {
        print "#ifdef QT_OPENGL_ES\n" if /OES/;
        print "    if (funcs->$name) {\n";
        print "        funcs->$name = ($type_name)\n";
        print "            context->getProcAddress(QLatin1String(\"$_\"));\n";
        print "    }\n";
        print "#endif\n" if /OES/;
    }
    if ($special_handling) {
        print "\n";
        print "    if (!funcs->$name)\n";
        print "        funcs->$name = $special_name;\n\n";
        if ($is_void) {
            print ("    ");
        } else {
            print ("    return ");
        }
        print "funcs->$name($_->{'argnamestr'});\n";
    } else {
        print "\n";
        print "    if (funcs->$name)\n";
        if ($is_void) {
            print ("        ");
        } else {
            print ("        return ");
        }
        print "funcs->$name($_->{'argnamestr'});\n";
        if ($is_void) {
            print "    else\n";
            print "        funcs->$name = $resolver_name;\n";
        } else {
            print "    funcs->$name = $resolver_name;\n";
            print "    return $_->{'returnType'}(0);\n";
        }
    }
    print "}\n\n";

    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;
print "#endif // !QT_OPENGL_ES_2\n\n";

# Generate the initialization code for QGLFunctionsPrivate.
print "QGLFunctionsPrivate::QGLFunctionsPrivate(const QGLContext *)\n";
print "{\n";
print "#ifndef QT_OPENGL_ES_2\n";
$last_shader_only = 0;
foreach ( @functions ) {
    my $inline = $_->{'inline'};
    next if $inline && $inline eq 'all';
    next if $inline && $inline eq 'all_diff';
    my $shader_only = ($_->{'shader_only'} && $_->{'shader_only'} eq 'yes');
    my $name = $_->{'varname'};
    my $resolver_name = $_->{'name'};
    $resolver_name =~ s/^gl/qglfResolve/;
    my @platforms = split /,\s*/,$inline;
    $shader_only = 1 if @platforms ~~ 'es1';
    #print "#ifndef QT_OPENGL_ES_1\n" if ($shader_only && !$last_shader_only);
    #print "#endif\n" if (!$shader_only && $last_shader_only);
    print "    $name = $resolver_name;\n";
    $last_shader_only = $shader_only;
}
#print "#endif\n" if $last_shader_only;
print "#endif // !QT_OPENGL_ES_2\n";
print "}\n\n";

#print Dumper(\@functions);
