#include <QVector>
#include <QFile>
#include <QTime>
#include "qgltexture2d.h"
#include "qgl.h"

#include "qglcolladafxeffect.h"
#include "qglcolladafxeffectfactory.h"
//#include "qglcolladafxeffectfactory_p.h"
#include "qglcolladafxeffect_p.h"


// The QGLColladaFxFactory class creates a list of QGLColladaFx objects from
// COLLADA FX information according to the 1.4.1 March 2008 Release from
// http://www.khronos.org/collada/

//#define DEBUG_QGL_COLLADA_PARSING
//#define DEBUG_MATERIALS

#define INDENT_SIZE 4

static int QGLColladaIndentLevel = 0;

// This is a simple RAII helper class to conveniently indent across
// various functions when building up the collada elements.
// Instantiate an indent to increment the indent level with that scope.
// add an indent object to the beginning of a string to indent it
// the correct amount.
class Indent
{
public:
    static int currentIndent() { return QGLColladaIndentLevel; };
    Indent()
    {
        ++QGLColladaIndentLevel;
    };
    ~Indent()
    {
        --QGLColladaIndentLevel;
    }
};

QString operator +(Indent&, QString string)
{
    return QString(QGLColladaIndentLevel * INDENT_SIZE, ' ') + string;
}

QString operator +(char c, Indent&)
{
    return c + QString(QGLColladaIndentLevel * INDENT_SIZE, ' ');
}

// xml convenience function - find the first end tag with the given tagname
// Note that this is not as smart as xml.skipCurrentElement(), and does
// not handle the case where (grand)children node share tagName
// With this caveat, can be called from within any (grand)child node.
static inline void findEndTag( QXmlStreamReader& xml, QString tagName )
{
    while( !xml.atEnd() && !( xml.tokenType() == xml.EndElement && xml.name() == tagName))
    {

        xml.readNext();
    }
}



/*!
    \internal
    Convenince function to find paramaters in \a stateStack that have an id, sid,
    or name that is equal to \a stringToMatch
    Note that this using this is not in line with the Collada specification.
  */
static inline QGLColladaParam* findParameterGenerous(StateStack* stateStack, QString stringToMatch)
{
    for ( int i = stateStack->count() - 1 ; 0 <= i; i-- )
    {
        QList<QGLColladaParam*> *params = stateStack->at( i );
        foreach( QGLColladaParam* param, *params )
        {
            if( param == 0 )
            {
                static bool warned = false;
                if( !warned )
                {
                    qWarning() << "Warning: null param in state stack while parsing collada.";
                    warned = true;
                }
                continue;
            }

            if(param->sid() == stringToMatch
               || param->id() == stringToMatch
               || (param->type() == QGLColladaParam::ImageType
                   && static_cast<QGLColladaImageParam*>(param)->name() == stringToMatch ))
                return param;
        }
    }
    return 0;
}



/*!
    Parse a collada 1.4 or 1.5 .dae file \a fileName, find the effects in the
    library_effects element, and convert these into a list of QGLColladaFxEffect objects
    suitable for use with Qt3d. Specific effects can be identified out of
    this list using their sid.

    This is the only function intended to be called from outside this class, e.g.:

    QList<QGLColladaFxEffect*> colladaEffects = QGLColladaFxEffectFactory::loadEffectsFromFile("myColladaFile.dae");

    \sa QGLColladaFxEffect
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::loadEffectsFromFile( const QString& fileName )
{
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly ))
    {
        qWarning() << "Warning: QGLColladaFxEffect failed to open file" << fileName;
        return QList<QGLColladaFxEffect*>();
    }

    QXmlStreamReader xml;
    xml.setDevice( &file );
    if(xml.tokenType() == QXmlStreamReader::Invalid)
        file.readLine();
    xml.setDevice( &file );

    return loadEffectsFromXml( xml );
}



QString QGLColladaFxEffectFactory::exportEffect(QGLShaderProgramEffect *effect, QString effectId, QString techniqueSid)
{
    QStringList result;

    result += "<?xml version=\"1.0\"?>";

    result += "<COLLADA "\
              "xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.5.0\" >";

    {
        Indent indent;
        result += indent + "<asset>";
        {
            Indent indent;
            QDateTime time = QDateTime::currentDateTime();
            result += indent + "<created>" + time.toString(Qt::ISODate) + "</created>";
            result += indent + "<modified>" + time.toString(Qt::ISODate) + "</modified>";
        }
        result += indent + "</asset>";

        result += indent + "<library_effects>";
        {
            Indent indent;
            result += indent + "<effect " + "id=\"" + effectId + "\">";
            result += glslProfileFromEffect(effect, techniqueSid);
            result += indent + "</effect>";
        }
        result += indent + "</library_effects>";
        result += "</COLLADA>";
        return result.join("\n");
    }
}


/*!
  \internal
  parse the top level \a xml from a .dae file and process the library_effects elements therein.
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::loadEffectsFromXml( QXmlStreamReader& xml )
{
    QList<QGLColladaFxEffect*> result;
    StateStack stateStack;

    while( !xml.atEnd() ) {
        xml.readNextStartElement();

        if( xml.name() == "library_effects" ) {
            result += processLibraryEffectsElement( xml , &stateStack );
        } else if (xml.name() == "library_images")
        {
            processLibraryImagesElement( xml, &stateStack );
        }
    }

    return result;
}



/*!
  \internal
*/
QGLColladaParam::QGLColladaParam(QString sid, float f) : mSid(sid)
        , mType(floatType)
        , mValue(1, f)
{
}



/*!
  \internal
*/
QGLColladaParam::QGLColladaParam( QString sid, float x, float y) : mSid(sid)
        , mType(float2Type)
        , mValue(2)
{
    mValue[0] = x;
    mValue[1] = y;
}



/*!
  \internal
*/
QGLColladaParam::QGLColladaParam( QString sid, float x, float y, float z ) : mSid(sid)
        , mType(float3Type)
        , mValue(3)
{
    mValue[0] = x;
    mValue[1] = y;
    mValue[2] = z;
}


QGLColladaParam::~QGLColladaParam()
{

}

/*!
  \internal
*/
QGLColladaParam::QGLColladaParam(QString sid, float x, float y, float z, float w) : mSid(sid)
        , mType(float4Type)
        , mValue(4)
{
    mValue[0] = x;
    mValue[1] = y;
    mValue[2] = z;
    mValue[3] = w;
}



/*!
  \internal
*/
int QGLColladaParam::type()
{
    return mType;
}



/*!
  \internal
*/
QString QGLColladaParam::sid()
{
    return mSid;
}



/*!
  \internal
*/
QVector<float> QGLColladaParam::value() {
    return mValue;
}



/*!
  \internal
*/
QGLColladaParam::QGLColladaParam(QString sid, int type) : mSid(sid)
        , mType(type)
{
}



/*!
  \internal
*/
QString QGLColladaParam::id()
{
    return mId;
}



/*!
  \internal
*/
QString QGLColladaParam::typeString(int type)
{
    const char* typeStringArray[] = {
        "UnkownType",
        "floatType",
        "float2Type",
        "float3Type",
        "float4Type",
        "Sampler2DType",
        "Texture2DType",
        "SurfaceType",
        "ImageType"};

    if( type < 0 || type > UserDefinedType)
        return("Unrecognized Type");
    else if (type >= UserDefinedType)
        return("UserDefinedType");
    else
        return typeStringArray[type];
}



/*!
  \internal
*/
QGLColladaTextureParam::QGLColladaTextureParam(QString sid, QGLTexture2D* texture) : QGLColladaParam(sid, Texture2DType)
{
    mTexture = texture;
}



/*!
  \internal
*/
QGLTexture2D* QGLColladaTextureParam::texture()
{
    Q_ASSERT( mType == Sampler2DType || mType == Texture2DType);
    return mTexture;
}



/*!
  \internal
*/
QString QGLColladaTextureParam::samplerSid()
{
    return sampler2DSid;
}



/*!
  \internal
*/
QGLColladaFxEffectFactory::QGLColladaFxEffectFactory()
{
}



/*!
  \internal
*/
QGLColladaSampler2DParam::QGLColladaSampler2DParam(QString sid, QGLTexture2D* texture)
    : QGLColladaParam(sid, QGLColladaParam::Sampler2DType)
    , mTexture(texture)
{
}



/*!
  \internal
*/
QGLColladaSampler2DParam::QGLColladaSampler2DParam(QString sid, QString sourceSid)
    : QGLColladaParam(sid, QGLColladaParam::Sampler2DType)
    , mTexture(0)
    , mSourceSid(sourceSid)
{
}



/*!
  \internal
*/
QString QGLColladaSampler2DParam::sourceSid()
{
    return mSourceSid;
}



/*!
  \internal
*/
QGLColladaSurfaceParam::QGLColladaSurfaceParam(QString sid) : QGLColladaParam(sid, QGLColladaParam::SurfaceType)
        , mInitFrom()
        , mFormat()
        , mFormatHint()
        , mSize()
        , mSizeVector( 3, 0 )
        , mViewportRatio( 1.0, 1.0 )
        , mMipLevels( 0 )
        , mMipMapGenerate( false )
        , mExtra()
        , mGenerator()
{
}



/*!
  \internal
*/
QGLColladaImageParam::QGLColladaImageParam(QString sid, QImage image)
    : QGLColladaParam(sid, QGLColladaParam::ImageType)
    , mImage(image)
{
}



/*!
  \internal
*/
QImage QGLColladaImageParam::image()
{
    return mImage;
}



/*!
  \internal
*/
QString QGLColladaImageParam::name()
{
    return mName;
}

QGLColladaCodeParam::QGLColladaCodeParam(QString sid, QString codeText)
    : QGLColladaParam(sid, QGLColladaParam::CodeType)
    , mCodeText(codeText)
{
}

const QString& QGLColladaCodeParam::code()
{
    return mCodeText;
};


/*!
  \internal
*/
QGLColladaSurfaceParam* QGLColladaFxEffectFactory::processSurfaceElement( QXmlStreamReader& xml , StateStack* stateStack, QString passedInSid)
{
    Q_UNUSED(stateStack);
    QXmlStreamAttributes attributes = xml.attributes();
    QString surfaceSid = attributes.value("sid").toString();
    // Surfaces are the only children of a newparam, but don't have their own sids.
    // for simplicity, if the surface doesn't have an sid, but the parent does, use the
    // parent's sid.

    if(surfaceSid.isEmpty() && !passedInSid.isEmpty())
        surfaceSid = passedInSid;
    QGLColladaSurfaceParam* result = new QGLColladaSurfaceParam( surfaceSid );
    xml.readNextStartElement();
    if( xml.name().toString().left(4) == "init" )
    {
        if(xml.name().toString() != "init_from")
            qWarning() << "Warning: only ""init_from"" supported in surface element ( line:" << xml.lineNumber() << ")";
        result->mInitFrom = xml.readElementText();
    }
    return result;
}



/*!
  \internal
*/
QVector<float> QGLColladaFxEffectFactory::processFloatList( QXmlStreamReader& xml )
{
    QVector<float> result;
    QString elementString = xml.readElementText();
    QStringList list = elementString.split( QRegExp( "\\s+" ), QString::SkipEmptyParts );
    bool ok;
    float f;
    foreach( QString string, list )
    {
        f = string.toFloat( &ok );
        if( ok )
            result.append(string.toFloat());
        else
            qWarning() << "Warning: malformed float ( line" << xml.lineNumber() << ")";
    }
    return result;
}



/*!
  \internal
*/
QGLColladaSampler2DParam* QGLColladaFxEffectFactory::processSampler2DElement( QXmlStreamReader& xml, StateStack* stateStack, QString passedInSid )
{
    Q_UNUSED(stateStack);
    QGLColladaSampler2DParam* result = 0;
    QXmlStreamAttributes attributes = xml.attributes();
    QString sid = attributes.value("sid").toString();
    if(sid.isEmpty() && !passedInSid.isEmpty())
    {
        sid = passedInSid;
    }

    xml.readNextStartElement();
    if( xml.name() == "source")
    {
        // Collada 1.4 Spec
        QString sourceSurfaceSid = xml.readElementText().trimmed();
        result = new QGLColladaSampler2DParam(sid, sourceSurfaceSid);
    }

    if( xml.name() == "instance_image" )
    {
        // Collada 1.5 Spec
        qWarning() << "collada 1.5 sampler elements not supported ( line:" << xml.lineNumber() << ")";
    }
    // exit cleanly, just in case.
    findEndTag( xml, "sampler2D");
    return result;
}



/*!
    \internal
    Parses and consumes a color collada element from \a xml.
*/
QColor QGLColladaFxEffectFactory::processColorElement( QXmlStreamReader& xml )
{

    QVector<float> floatList = processFloatList( xml );
    if( floatList.size() < 4 )
    {
        qWarning() << "Warning: Malformed color element ( line" << xml.lineNumber() << ")";
        return QColor( 0, 0, 0, 255 );
    }
    QColor result;
    result.setRgbF( floatList.at( 0 )
                    , floatList.at( 1 )
                    , floatList.at( 2 )
                    , floatList.at( 3 ) );
    return result;
}



/*!
    \internal
    Parses and consumes an fx_common_color_or_texture_type collada element from \a xml.
*/
QColor QGLColladaFxEffectFactory::processColorOrTextureElement( QXmlStreamReader& xml )
{
    if( xml.name() == "color")
    {
        return processColorElement( xml );
    } else if( xml.name() == "texture" )
    {
        qWarning() << "Warning: texture element not supported ( line" << xml.lineNumber()<<")";
    } else
    {
        qWarning() << "Color or Texture expected ( line" << xml.lineNumber() << ")";
    }
    xml.skipCurrentElement();
    return( QColor( 0, 0, 0,255 ));
}



/*!
    \internal
    Parses an fx_common_float_or_param_type collada element from \a xml.
    Always consumes the element.
*/
float QGLColladaFxEffectFactory::processParamOrFloatElement( QXmlStreamReader& xml )
{
    if( xml.name() == "param" )
    {
        qWarning() << "Warning: params not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        return 0.0;
    } else
    {
        return xml.readElementText().toFloat();
    }
}



/*!
    Parses and consumes a library_images collada element pointed to by \a xml,
    and pushes any images found onto the \a stateStack for use in resolving
    elements later.
*/
void QGLColladaFxEffectFactory::processLibraryImagesElement( QXmlStreamReader& xml, StateStack* stateStack )
{
    QList<QGLColladaParam*>* list = new QList<QGLColladaParam*>;
    xml.readNextStartElement();

    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: effect asset handling not supported in library_images element ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "image" && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        QGLColladaParam* imageParam = processImageElement( xml , stateStack );
        list->append( imageParam );
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    stateStack->push_back(list);
}


/*!
  \internal
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processLibraryEffectsElement( QXmlStreamReader& xml, StateStack* stateStack )
{
    QList<QGLColladaFxEffect*> result;
    // A collada library_effects is
    //    0 or 1 <asset>
    //    1 or more <effect>,
    //    0 or more <extra>;
    xml.readNextStartElement();

    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: effect asset handling not supported in effects library ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "effect" && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        result += processEffectElement( xml , stateStack );
        xml.readNextStartElement();
    }

    while( xml.readNextStartElement() && xml.name() == "extra" )
    {
        qWarning() << "Warning: extra element not handled in effects library ( line" << xml.lineNumber() << ")";
    }

    // be sure to exit cleanly
    findEndTag(xml, "library_effects");
    return result;
}



/*!
  \internal
  Parses and consumes an effect element from \a xml.
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processEffectElement( QXmlStreamReader& xml, StateStack* stateStack )
{
    Q_UNUSED(stateStack);
    //    An effect element is:
    //    0 or 1 <annotate>
    //    0 or more newparam
    //    1 or more profile ( either <profile_BRIDGE>, <profile_CG>, <profile_GLES>, <profile_GLES2>, <profile_GLSL>, or <profile_COMMON>
    //    0 or more <extra>

    QList<QGLColladaFxEffect*> result;
    xml.readNextStartElement();

    if( xml.name() == "annotate" )
    {
        qWarning() << "effect annotation not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    QList<QGLColladaParam*>* params = new QList<QGLColladaParam*>;
    while( xml.name() == "newparam" && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        QGLColladaParam* result = processNewparamElement( xml , stateStack );
        if( result != 0 )
            params->append( result );

        xml.readNextStartElement();
    }
    stateStack->push_back( params );

    // find any of the profile_* elements defined in the spec
    QRegExp profileRegExp( "profile_(BRIDGE|CG|GLES2?|GLSL|COMMON)" );
    while( profileRegExp.indexIn( xml.name().toString() ) == 0 && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        result += processProfileElement( xml, stateStack );
        xml.readNextStartElement();
    }

    findEndTag(xml, "effect");
    return result;
}



/*!
  \internal
*/
QList<QGLColladaFxEffect*> QGLColladaFxEffectFactory::processProfileElement( QXmlStreamReader& xml, StateStack* stateStack )
{
    // A profile_GLES2 element is:
    // 0 or 1 <asset>
    // 0 or more <code> or <include>
    // 0 or more <newparam>
    // 1 or more <technique>

    // A profile_GLSL element is:
    // 0 or 1 <asset>
    // 0 or more <code>
    // 0 or more <include>
    // 0 or more <newparam>
    // 1 or more <technique>
    // 0 or more <extra>

    // A profile_COMMON element is
    // an optional id element
    // 0 or 1 <asset>
    // 0 or more <newparam>
    // 1 <technique>
    // 0 or more <extra>

    // Note: techniques need to be handled differently for different profiles

    QString rootNodeString = xml.name().toString();
    QList<QGLColladaFxEffect*> result;
    QList<QGLColladaParam*>* params = new QList<QGLColladaParam*>;
    stateStack->push_back(params);

    xml.readNextStartElement();
    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: asset element not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    if(rootNodeString == "profile_GLSL")
    {
        while ( xml.name() == "code" )
        {
            QString codeSid = xml.attributes().value("sid").toString();
            QString codeText = xml.readElementText();

            params->append(new QGLColladaCodeParam(codeSid, codeText));

            findEndTag(xml, "code");
            xml.readNextStartElement();
        }


        while ( xml.name() == "include" )
        {
            QString includeSid = xml.attributes().value("sid").toString();
            QString includeUrl = xml.attributes().value("url").toString();

            // create an include param?
            qWarning() << "Warning: include element not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";

            findEndTag(xml, "include");
            xml.readNextStartElement();
        }

    }

    while( xml.tokenType() == QXmlStreamReader::StartElement &&
           ( xml.name() == "newparam" || xml.name() == "image" ))
    {
        if( xml.name() == "newparam" )
            params->append( processNewparamElement( xml , stateStack ));
        else if( xml.name() == "image" )
            params->append( processImageElement( xml , stateStack ));

        xml.readNextStartElement();
    }


    while ( xml.name() == "technique" )
    {
        result.append(
                processTechniqueElement( xml, stateStack, rootNodeString ));
        xml.readNextStartElement();
        // only 1 technique in profile_COMMON
        if( rootNodeString == "profile_COMMON")
        {
            break;
        }
    };

    while ( xml.name() == "extra" )
    {
        qWarning() << "extra elements currently not supported in " << rootNodeString << "elements ( line" << xml.lineNumber() << ")";
        findEndTag( xml, "extra" );
        xml.readNextStartElement();
    };

    stateStack->pop_back();
    findEndTag( xml, rootNodeString );
    return result;
}


QGLColladaParam* QGLColladaFxEffectFactory::processPassElement( QXmlStreamReader& xml, StateStack* stateStack, QGLColladaFxEffect* effect )
{
    QGLColladaParam* result = 0;
    // a profile_GLSL pass is:
    // 0 or 1 <annotate>
    // 0 or 1 <states>
    // 0 or 1 <program> (CG, GLES2 or GLSL only)
    // 0 or 1 <evaluate>
    // 0 or 1 <extra>

    xml.readNextStartElement();

    if( xml.name() == "annotate" )
    {
        qWarning() << "Warning: annotate element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, "annotate" );
        xml.readNextStartElement();
    }

    if( xml.name() == "states" )
    {
        qWarning() << "Warning: states element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, "states" );
        xml.readNextStartElement();
    }

    // 0 or 1 <program> (CG, GLES2 or GLSL only)
    if( xml.name() == "program" )
    {
        processProgramElement( xml, stateStack, effect );
        findEndTag( xml, "program" );
        xml.readNextStartElement();
    }

    // 0 or 1 <evaluate>
    if( xml.name() == "evaluate" )
    {
        qWarning() << "Warning: evaluate element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, "evaluate" );
        xml.readNextStartElement();
    }

    // 0 or more <extra>
    while ( xml.name() == "extra" )
    {
        qWarning() << "Warning: extra element not supported ( line" << xml.lineNumber() << ")";
        findEndTag( xml, "extra" );
        xml.readNextStartElement();
    }

    findEndTag( xml, "pass");
    return result;
}

QGLColladaFxEffect* QGLColladaFxEffectFactory::processTechniqueElement( QXmlStreamReader& xml, StateStack* stateStack, QString &profileName )
{
    // A 1.4 technique is:
    // 0 or 1 <asset>
    // 0 or more <newparam> or <image> (in any order)
    // 0 or more of <constant>, <lambert>, <phong>, <blinn>
    // 0 or more <extra>

    // A 1.5 profile_COMMON technique is:
    // 0 or 1 <asset>
    // exactly 1 of <blinn>, <constant>, <lambert>, or <phong>
    // 0 or more <extra>

    // a profile_GLSL technique is:
    // 0 or 1 <asset>
    // 0 or more <annotate>
    // 1 or more <pass>
    // 0 or more <extra>

    QGLColladaFxEffect* effect = new QGLColladaFxEffect();

    QXmlStreamAttributes attributes = xml.attributes();
    effect->setSid( attributes.value( "sid" ).toString() );
    QStringRef id = attributes.value( "id" );
    Q_UNUSED( id );

    xml.readNextStartElement();

    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: asset element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "annotate" && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        qWarning() << "Warning: annotate element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Collada specifies exactly one of blinn, lambert, constant or phong
    // If the effect is malformed, default QGLMaterial will be used.
    QGLMaterial* material = new QGLMaterial;

    if( profileName == "profile_COMMON" &&
        (xml.name() == "blinn" || xml.name() == "phong" ||
         xml.name() == "constant"|| xml.name() == "lambert") )
    {
        if( xml.name() == "blinn" )
        {
            effect->setLighting( QGLColladaFxEffect::BlinnLighting );
        } else if ( xml.name() == "phong" ) {
            effect->setLighting( QGLColladaFxEffect::PhongLighting );
        } else if ( xml.name() == "constant" ) {
            effect->setLighting( QGLColladaFxEffect::ConstantLighting );
        } else if ( xml.name() == "lambert" ) {
            effect->setLighting( QGLColladaFxEffect::LambertLighting );
        }

        // TODO: get appropriate shader fragments for the specified lighting models
        if(  xml.readNextStartElement() )
        {
            // a blinn element is 0 or 1 of each of:
            //        <emission>
            //        <diffuse>
            //        <specular>
            //        <shininess>
            //        <reflectivity>
            //        <transparent>
            //        <transparency>
            //        <index_of_refraction>

            if( xml.name() == "emission" )
            {
                if( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if(xml.name() == "color")
                    {
                        material->setEmittedLight( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set emitted light to " << material->emittedLight();
#endif
                    }
                    else if( xml.name() == "texture")
                    {
                        effect->d->emissiveTexture = processTextureElement( xml, stateStack );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set emissive texture to " << effect->d->emittedTexture;
#endif
                    } else if( xml.name() == "param")
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "ambient" )
            {
                if( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if(xml.name() == "color")
                    {
                        material->setAmbientColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set ambient color to " << material->ambientColor();
#endif
                    }
                    else if( xml.name() == "texture")
                    {
                        effect->d->ambientTexture = processTextureElement( xml, stateStack );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set ambient texture to " << effect->d->ambientTexture;
#endif
                    } else if( xml.name() == "param")
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "diffuse" )
            {
                if( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if(xml.name() == "color")
                    {
                        material->setDiffuseColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set diffuse color to " << material->diffuseColor();
#endif
                    }
                    else if( xml.name() == "texture")
                    {
                        effect->d->diffuseTexture = processTextureElement( xml, stateStack );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set diffuse texture to " << effect->d->diffuseTexture;
#endif
                    } else if( xml.name() == "param")
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "specular" )
            {
                if( xml.readNextStartElement() )
                {
                    // handle color or texture element:
                    if(xml.name() == "color")
                    {
                        material->setSpecularColor( processColorElement( xml ));
#ifdef DEBUG_MATERIALS
                        qDebug() << "set specular color to " << material->specularColor();
#endif
                    }
                    else if( xml.name() == "texture")
                    {
                        effect->d->specularTexture = processTextureElement( xml, stateStack );
#ifdef DEBUG_MATERIALS
                        qDebug() << "set specular texture to " << effect->d->specularTexture;
#endif
                    } else if( xml.name() == "param")
                    {
                        qWarning() << "params not supported in lighting elements ( line" << xml.lineNumber() << ")";
                    }
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "shininess" )
            {
                if( xml.readNextStartElement() )
                {
                    float shininess = processParamOrFloatElement( xml );
                    if( 0.0 < shininess && shininess < 1.0 )
                    {
                        qWarning() << "Warning: Blinn-Torrance-Sparrow specular lighting not suported ( line" << xml.lineNumber()
                                << "), converting to Blinn-Phong specular model";
                        material->setShininess( int( shininess * 128.0 ));
                    }
                    else
                        material->setShininess( int( shininess ) );
#ifdef DEBUG_MATERIALS
                    qDebug() << "set shininess to " << material->shininess();
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "reflective" )
            {
                qWarning() << "Warning reflective not supported ( line" << xml.lineNumber() << ")";
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "reflectivity" )
            {
                qWarning() << "Warning: reflectivity not supported ( line" << xml.lineNumber() << ")";
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "transparent" )
            {
                // TODO: color or texture
                if( xml.readNextStartElement() )
                {
                    if(xml.name() == "texture")
                    {
                        QGLTexture2D* transparentTexture = processTextureElement( xml , stateStack );
                        Q_UNUSED(transparentTexture);
                        qWarning() << "Warning: transparent not supported ( line" << xml.lineNumber() << ")";
#ifdef DEBUG_MATERIALS
                        qDebug() << "unused transparent texture" << transparentTexture;
#endif
                    }
                    else if(xml.name() == "color")
                    {
                        QColor transparent = processColorElement( xml );
                        Q_UNUSED( transparent );
                        qWarning() << "Warning: transparent not supported ( line" << xml.lineNumber() << ")";
#ifdef DEBUG_MATERIALS
                        qDebug() << "unused transparent color of " << transparent;
#endif
                    }

                }
            }
            xml.skipCurrentElement();
            xml.readNextStartElement();
        }

        if( xml.name() == "transparency" )
        {
            if( xml.readNextStartElement() )
            {
                float transparency = processParamOrFloatElement( xml );
                if( transparency < 1.0 )
                {
                    qWarning() << "Warning: transparency not supported";
                }
            }
            xml.skipCurrentElement();
            xml.readNextStartElement();
        }

        if( xml.name() == "index_of_refraction" )
        {
            if( xml.readNextStartElement() )
            {
                float indexOfRefraction = processParamOrFloatElement( xml );
                Q_UNUSED( indexOfRefraction );
                qWarning() << "Warning: index_of_refraction not supported ( line" << xml.lineNumber() << ")";
                xml.skipCurrentElement();
            }
            xml.skipCurrentElement();
            xml.readNextStartElement();
        }
        xml.skipCurrentElement();
        xml.readNextStartElement();

        effect->setMaterial( material );

        switch(effect->lighting())
        {
        case QGLColladaFxEffect::PhongLighting:
        case QGLColladaFxEffect::LambertLighting:
            qWarning() << "Warning: requested lighting not supported, using Blinn-Phong instead";
        case QGLColladaFxEffect::BlinnLighting:
            effect->addBlinnPhongLighting();
        case QGLColladaFxEffect::ConstantLighting:
        case QGLColladaFxEffect::NoLighting:
        default:
            break;
        }
    };

    while( xml.name() == "pass" && xml.tokenType() == QXmlStreamReader::StartElement )
    {
        processPassElement( xml, stateStack, effect);
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Make sure to exit cleanly
    findEndTag( xml, "technique" );

    return effect;
}



QGLColladaParam* QGLColladaFxEffectFactory::processNewparamElement( QXmlStreamReader& xml, StateStack* stateStack)
{
    QXmlStreamAttributes attributes = xml.attributes();
    QGLColladaParam* result = 0;
    if(xml.readNextStartElement())
    {
        if(xml.name().toString().left(5) == "float")
        {

            QVector<float> floatValues = processFloatList( xml )            ;

            if( xml.name() == "float" && floatValues.count() > 0 )
            {
                new QGLColladaParam(attributes.value("sid").toString()
                                    , floatValues.at(0) );
            }
            else if( xml.name() == "float2" && floatValues.count() > 1 )
            {
                new QGLColladaParam(attributes.value("sid").toString()
                                    , floatValues.at(0)
                                    , floatValues.at(1));

            }
            else if( xml.name() == "float3" && floatValues.count() > 2 )
            {
                new QGLColladaParam(attributes.value("sid").toString()
                                    , floatValues.at(0)
                                    , floatValues.at(1)
                                    , floatValues.at(2) );
            }
            else if( xml.name() == "float4" && floatValues.count() > 3 )
            {
                new QGLColladaParam(attributes.value("sid").toString()
                                    , floatValues.at(0)
                                    , floatValues.at(1)
                                    , floatValues.at(2)
                                    , floatValues.at(3) );
            }
        } else if( xml.name() == "sampler2D" )
        {
            result = processSampler2DElement( xml , stateStack, attributes.value("sid").toString() );
        } else if( xml.name() == "surface" )
        {
            result = processSurfaceElement( xml, stateStack, attributes.value("sid").toString());
        } else {
            qWarning() << "unrecognized paramter type ( line:" << xml.lineNumber() << ")";
            findEndTag( xml, "newparam" );
            return 0;
        }
    }
    findEndTag(xml, "newparam");
    return result;
}




/*!
    \internal
    Parses and consumes a texture collada element from \a xml.
*/
QGLTexture2D* QGLColladaFxEffectFactory::processTextureElement( QXmlStreamReader& xml , StateStack* stateStack)
{
    QGLTexture2D* result = new QGLTexture2D();
    QXmlStreamAttributes attributes = xml.attributes();

    foreach(QXmlStreamAttribute attribute, attributes)
    {
        if( attribute.name() == "texture" )
        {
            QString paramName = attribute.value().toString();

            // in Collada Fx Textures must reference a previously defined sampler2D param
            QGLColladaParam* sourceParam = findParameterGenerous(stateStack, paramName);
            if(sourceParam != 0 && sourceParam->type() == QGLColladaParam::Sampler2DType)
            {
                QGLColladaSampler2DParam* sourceSamplerParam = static_cast<QGLColladaSampler2DParam*>( sourceParam );

                // in Collada Fx 1.4, a sampler must have a source surface, so find this now.
                QGLColladaParam* sourceParamForSampler = findParameterGenerous(stateStack, sourceSamplerParam->sourceSid());
                if( sourceParamForSampler != 0 && sourceParamForSampler->type() == QGLColladaParam::SurfaceType)
                {
                    QGLColladaSurfaceParam* sourceSurface = static_cast<QGLColladaSurfaceParam*> (sourceParamForSampler);
                    QString sourceImageName = sourceSurface->mInitFrom;

                    // Look for a source file:
                    QGLColladaParam* sourceParamForSurface = findParameterGenerous(stateStack, sourceImageName);
                    if(sourceParamForSurface != 0 && sourceParamForSurface->type() == QGLColladaParam::ImageType)
                    {
                        QGLColladaImageParam* sourceImageParam= static_cast<QGLColladaImageParam*>( sourceParamForSurface );
                        result->setImage( sourceImageParam->image() );
                    }
                }
            }
            else
            {
                qWarning() << "failed to find a source sampler for texture ( line" << xml.lineNumber() << ")";
            }
        } else if( attribute.name() == "texcoord" )
        {
            // TODO: Create a repository for this sort of data, to be used in creating the shader progams later
            // I'm pretty sure the effect is going to need to be passed in.
            qWarning() << "texcoord not supported yet: " << attribute.name() << attribute.value() << " ( line" << xml.lineNumber() << ")";
        } else if (attribute.name() == "extra")
        {
            qWarning() << "extra elements in texture elements not supported ( line" << xml.lineNumber() << ")";
        }
    }
    xml.skipCurrentElement();

    return result;
}



/*!
  \internal
  Parses and consumes an image element from \a xml.
*/
QGLColladaImageParam* QGLColladaFxEffectFactory::processImageElement( QXmlStreamReader& xml, StateStack* stateStack )
{
    Q_UNUSED(stateStack);
    // 1.4 has a bunch of optional values in the attributes:
    QString sid = xml.attributes().value("sid").toString();
    QString id = xml.attributes().value("id").toString();
    QString name = xml.attributes().value("name").toString();

    QString height = xml.attributes().value("height").toString();
    QString width = xml.attributes().value("width").toString();
    QString depth = xml.attributes().value("depth").toString();

    Q_UNUSED(height);
    Q_UNUSED(width);
    Q_UNUSED(depth);

    QGLColladaImageParam* result = 0;

    xml.readNextStartElement();
    if(xml.name() == "asset")
    {
        qWarning() << "asset element not supported in image elements ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    if(xml.name() == "init_from")
    {
        QString imageFileName = xml.readElementText().trimmed();
        QImage image(imageFileName);
        result = new QGLColladaImageParam( sid, image );
    }

    // exit cleanly
    findEndTag( xml, "image");

    if(result != 0)
    {
        result->mId = id;
        result->mName = name;
    }
    return result;
}

QStringList QGLColladaFxEffectFactory::glslProfileFromEffect(QGLShaderProgramEffect* effect, QString sid)
{
    Q_UNUSED(effect)
    Indent indent;
    QStringList result;
    result += indent + "<profile_GLSL>";
    {
        result += generateCodeElements(effect, sid);
        result += indent + "<technique " + "sid=\"" + sid + "\"" + ">";
        {
            Indent indent;
            result += indent + "<pass>";
            result += generateProgramElement(effect, sid);
            result += indent + "</pass>";
        }
        result += indent + "</technique>";
    }
    result += indent + "</profile_GLSL>";

    return result;
}

QStringList QGLColladaFxEffectFactory::generateProgramElement(QGLShaderProgramEffect* effect, QString techniqueSid)
{
    QStringList result;
    QString vertexShaderRefSid = "VertexShaderRefSidRefsCodeOrIncludeAtProfileOrEffectLevel";
    QString fragmentShaderRefSid = "FragmentShaderRefSidRefsCodeOrIncludeAtProfileOrEffectLevel";
    Indent indent;
    result += indent + "<program>";
    result += generateShaderElement(effect, techniqueSid + "VertexShader", techniqueSid + "FragmentShader");
    // 0 or more
    result += generateBindAttributeElement( effect );
    // 0 or more
    result += generateBindUniformElements( effect );
    result += indent + "</program>";
    return result;
}

QStringList QGLColladaFxEffectFactory::generateShaderElement( QGLShaderProgramEffect* effect, QString vertexShaderRefSid, QString fragmentShaderRefSid )
{
    Q_UNUSED(effect);
    QStringList result;
    Indent indent;
    result += indent + "<shader stage=\"VERTEX\">";
    {
        Indent indent;
        result += indent + "<sources>";
        {
            // 0 or more <import> elements
            Indent indent;
            result += indent + "<import ref=\"" + vertexShaderRefSid + "\"/>";
        }
        result += indent + "</sources>";
        // 0 or <extra> elements;
    }
    result += indent + "</shader>";

    result += indent + "<shader stage=\"FRAGMENT\">";
    {
        Indent indent;
        result += indent + "<sources>";
        {
            Indent indent;
            result += indent + "<import ref=\"" + fragmentShaderRefSid + "\"/>";
        }
        result += indent + "</sources>";
        // <extra> element(s) here if necessary;
    }
    result += indent + "</shader>";
    return result;
}

QStringList QGLColladaFxEffectFactory::generateBindAttributeElement( QGLShaderProgramEffect* effect )
{
    // Currently no need for bind_attribute elements.
    Q_UNUSED(effect);
    QStringList result;
    //    Indent indent;
    //    result += indent + "<bind_attribute>";
    //    result += indent + "</bind_attribute>";
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, QString ref)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + "<bind_uniform symbol=\"" + symbol + "\">";
    {
        Indent indent;
        result += indent + "<param ref=\"" + ref + "\">";
    }
    result += indent + "</bind_uniform>";
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, const QVector3D& value)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + "<bind_uniform symbol=\"" + symbol + "\">";
    {
        Indent indent;
        result += indent + QString("<float3> %1 %2 %3 </float3>").arg(value.x()).arg(value.y()).arg(value.z());
    }
    result += indent + "</bind_uniform>";
    return result;
}

QStringList generateBindUniformParamElement( QString symbol, const QColor& value)
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;
    result += indent + "<bind_uniform symbol=\"" + symbol + "\">";
    {
        Indent indent;
        result += indent + QString("<float3> %1 %2 %3 </float3>").arg(value.redF()).arg(value.greenF()).arg(value.blueF());
    }
    result += indent + "</bind_uniform>";
    return result;
}


QStringList QGLColladaFxEffectFactory::generateBindUniformElements( QGLShaderProgramEffect* effect )
{
    QStringList result;
    if(effect == 0)
        return result;
//    // 0 or more <bind_uniform> elements
    if(effect->material() != 0)
    {
        QGLMaterial* material = effect->material();
        result += generateBindUniformParamElement( "exampleFloat3Symbol", QVector3D(0.1, 0.2, 0.3) );
        result += generateBindUniformParamElement( "ambientColor", material->ambientColor());
        result += generateBindUniformParamElement( "diffuseColor", material->diffuseColor());
        result += generateBindUniformParamElement( "emittedLight", material->emittedLight());
        result += generateBindUniformParamElement( "objectName", material->objectName()); // ?
        result += generateBindUniformParamElement( "shininess", material->shininess());
        result += generateBindUniformParamElement( "specularColor", material->specularColor());
    }
    effect->supportsPicking();

    // TODO: Find and store effect uniforms
//    effect->bindProgramUniforms();

    // Example uniforms
//    result += generateBindUniformParamElement( "exampleRefSymbol", QString("exampleRef"));
//    result += generateBindUniformParamElement( "exampleFloat3Symbol", QVector3D(0.1, 0.2, 0.3) );
    return result;
}

QStringList QGLColladaFxEffectFactory::generateCodeElements( QGLShaderProgramEffect* effect, QString baseSid )
{
    QStringList result;
    // 0 or more <bind_uniform> elements
    Indent indent;

    // put all this on one line to avoid adding carriage returns to the
    // shader programs
    result += indent + "<code sid=\"" + baseSid + "VertexShader" + "\">"
              + effect->vertexShader() + "</code>";

    result += indent + "<code sid=\"" + baseSid + "FragmentShader" + "\">"
              + effect->fragmentShader() + "</code>";

    return result;
}

void QGLColladaFxEffectFactory::processProgramElement( QXmlStreamReader& xml, StateStack* stateStack, QGLColladaFxEffect* effect )
{
    // A profile_GLSL shader element is
    // 0 or more <shader>
    // 0 or more <bind_attribute>
    // 0 or more <bind_uniform>

    xml.readNextStartElement();

    while( xml.name() == "shader" )
    {
        // in profile_GLSL a shader is
        // exactly 1  <source>
        // 0 or more <extra>

        QString stage = xml.attributes().value("stage").toString();
        xml.readNextStartElement();
        if( xml.name() == "sources" )
        {
            // a <sources> element is
            // 1 or more <inline> elements
            // 0 or more <import> elements
            // Note: child elements can appear in any order

            xml.readNextStartElement();
            while( (xml.name() == "inline" || xml.name() == "import") && xml.tokenType() == QXmlStreamReader::StartElement)
            {
                if( xml.name() == "import")
                {
                    QString ref = xml.attributes().value("ref").toString();

                    QXmlStreamAttribute attr;
                    if(xml.attributes().count())
                    {
                        attr = xml.attributes().first();
                    }
                    QGLColladaParam* param = findParameterGenerous(stateStack, ref);
                    if(param  == 0 || param->type() != QGLColladaParam::CodeType)
                    {
                        qWarning() << "null or unexpected parameter found in import element ( line"
                                <<  xml.lineNumber()<<")";
                    }
                    else
                    {
                        QGLColladaCodeParam* codeParam = static_cast<QGLColladaCodeParam*>(param);
                        if(stage == "VERTEX")
                        {
                            effect->setVertexShader( codeParam->code() );
                        }
                        else if (stage == "FRAGMENT")
                        {
                            effect->setFragmentShader( codeParam->code() );
                        } else
                        {
                            qWarning() << "unrecognized shader stage: "
                                    << stage << " ( line" <<  xml.lineNumber()<<")";
                        }
                    }

                } else if( xml.name() == "inline")
                {

                }
                xml.readNextStartElement();
            }
        } else {
            qWarning() << "collada parsing error. expected <sources> element ( line"
                    <<  xml.lineNumber()<<")";
        }

        if(xml.name() == "extra")
            qWarning() << "Warning: extra element not supported in profile_GLSL <shader> element ( line" << xml.lineNumber()<<")";

        findEndTag ( xml, "shader");
        xml.readNextStartElement();
    }

    while( xml.name() == "bind_attribute" )
    {
        qWarning() << "Warning: bind_attribute element not supported ( line" << xml.lineNumber()<<")";
        findEndTag ( xml, "bind_attribute");
        xml.readNextStartElement();
    }

    while( xml.name() == "bind_uniform" )
    {
        qWarning() << "Warning: bind_uniform element not supported ( line" << xml.lineNumber()<<")";
        findEndTag ( xml, "bind_uniform");
        xml.readNextStartElement();
    }

    findEndTag(xml, "program");
    return;
}
