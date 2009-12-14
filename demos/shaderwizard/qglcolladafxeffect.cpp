#include "qglcolladafxeffect.h"
#include <QString>
#include <QXmlStreamReader>
#include <QFile>

#include <QDebug>

// This class reads COLLADA FX information according to the 1.5.0 April 2008 Release
// from http://www.khronos.org/collada/

#define DEBUG_MATERIALS

// xml convenience functions
static inline void findEndTag( QXmlStreamReader& xml, QString tagName )
{
    while( !xml.atEnd() && !( xml.tokenType() == xml.EndElement && xml.name() == tagName))
    {
        xml.readNext();
    }
}


/*!
    \internal
    Parses and consumes a color collada element from \a xml.
*/
static inline QColor parseColorElement( QXmlStreamReader& xml )
{
    QString colorString = xml.readElementText();
    QStringList colors = colorString.split( QRegExp( "\\s+" ), QString::SkipEmptyParts );
    if( colors.size() < 4 )
    {
        qWarning() << "Warning: Malformed color element ( line" << xml.lineNumber() << ")";
        return QColor( 0, 0, 0, 255 );
    }
    QColor result;
    result.setRgbF( colors.at( 0 ).toFloat()
                    , colors.at( 1 ).toFloat()
                    , colors.at( 2 ).toFloat()
                    , colors.at( 3 ).toFloat() );
    return result;
}


/*!
    \internal
    Parses and consumes an fx_common_color_or_texture_type collada element from \a xml.
*/
static inline QColor colorOrTexture( QXmlStreamReader& xml )
{
    if( xml.name() == "color")
    {
        return parseColorElement( xml );
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

static float paramOrFloat( QXmlStreamReader& xml )
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



class QGLColladaFXEffectPrivate
{
public:
    QGLColladaFXEffectPrivate() : id()
            , name()
    {
    }

    ~QGLColladaFXEffectPrivate()
    {
    }

    QString id;
    QString name;
};



QGLColladaFXEffect::QGLColladaFXEffect() : QGLShaderProgramEffect()
        , d( new QGLColladaFXEffectPrivate )
{
}



QList<QGLColladaFXEffect*> QGLColladaFXEffect::loadEffectsFromFile( const QString& fileName )
{
    QList<QGLColladaFXEffect*> result;
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly ))
    {
        qWarning() << "Warning: QGLColladaFXEffect failed to open "<<fileName;
        return result;
    }

    QXmlStreamReader xml;
    xml.setDevice( &file );
    while( !xml.atEnd() ) {
        xml.readNextStartElement();
        if( xml.name() == "library_effects" ) {
            result += processLibraryEffectsElement( xml );
        }
    }

    return result;
}



QList<QGLColladaFXEffect*> QGLColladaFXEffect::processLibraryEffectsElement( QXmlStreamReader& xml )
{
    QList<QGLColladaFXEffect*> result;

    // A collada library_effects is
//    0 or 1 <asset>
//    1 or more <effect>,
//    0 or more <extra>;
    xml.readNextStartElement();

    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: effect asset handling not supported";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "effect" )
    {
        result += processEffectElement( xml );
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
QList<QGLColladaFXEffect*> QGLColladaFXEffect::processEffectElement( QXmlStreamReader& xml )
{
    //    An effect element is:
    //    0 or 1 <annotate>
    //    0 or more newparam
    //    1 or more profile ( either <profile_BRIDGE>, <profile_CG>, <profile_GLES>, <profile_GLES2>, <profile_GLSL>, or <profile_COMMON>
    //    0 or more <extra>

    QList<QGLColladaFXEffect*> result;
    xml.readNextStartElement();

    if( xml.name() == "annotate" )
    {
        qWarning() << "effect annotation not supported";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "newparam" )
    {
        processNewparamElement( xml );
        xml.readNextStartElement();
    }

    QRegExp profileRegExp( "profile_(BRIDGE|CG|GLES2?|GLSL|COMMON)" );
    while( profileRegExp.indexIn( xml.name().toString() ) == 0 )
    {
        result += processProfileElement( xml );
        xml.readNextStartElement();
    }

    findEndTag(xml, "effect");
    return result;
}



QList<QGLColladaFXEffect*> QGLColladaFXEffect::processProfileElement( QXmlStreamReader& xml )
{
    //    A profile_GLES2 element is:
    //    0 or 1 <asset>
    //    0 or more <code> or <include>
    //    0 or more <newparam>
    //    1 or more <technique>

    QString rootNodeString = xml.name().toString();
    QList<QGLColladaFXEffect*> result;

    while( xml.readNextStartElement() )
    {
        if( xml.name() == "technique" )
            result.append(processTechniqueElement( xml ));
        else
            xml.skipCurrentElement();
    }

    return result;
}



QGLColladaFXEffect* QGLColladaFXEffect::processTechniqueElement( QXmlStreamReader& xml )
{
    //    A technique is:
    //    0 or 1 <asset>
    //    0 or more <annotate>
    //    any 1 of <blinn>, <constant>, <lambert>, <phong>
    //    1 or more <pass>
    //    0 or more <extra>
    QXmlStreamAttributes attributes = xml.attributes();

    QStringRef sid = attributes.value( "sid" );
    QStringRef id = attributes.value( "id" );
    Q_UNUSED( sid );
    Q_UNUSED( id );

    xml.readNextStartElement();

    QGLColladaFXEffect* result = new QGLColladaFXEffect;
    if( xml.name() == "asset" )
    {
        qWarning() << "Warning: asset element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    while( xml.name() == "annotate" )
    {
        qWarning() << "Warning: annotate element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Collada specifies one of blinn, lambert, constant or phong, so assume
    // we will get one.  If the effect is malformed, default QGLMaterialParameters
    // will be used.
    QGLMaterialParameters* material = new QGLMaterialParameters;

    if( xml.name() == "blinn" || xml.name() == "phong" || xml.name() == "constant"|| xml.name() == "lambert")
    {
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
                // color or texture
                if( xml.readNextStartElement() )
                {
                    material->setEmittedLight( colorOrTexture( xml ));
#ifdef DEBUG_MATERIALS
                    qDebug() << "set emitted light to " << material->diffuseColor();
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "ambient" )
            {
                // color or texture
                if( xml.readNextStartElement() )
                {
                    material->setAmbientColor( colorOrTexture( xml ));
#ifdef DEBUG_MATERIALS
                    qDebug() << "set ambient color to " << material->diffuseColor();
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "diffuse" )
            {
                // color or texture
                if( xml.readNextStartElement() )
                {
                    material->setDiffuseColor( colorOrTexture( xml ));
#ifdef DEBUG_MATERIALS
                    qDebug() << "set diffuse color to " << material->diffuseColor();
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }
            
            if( xml.name() == "specular" )
            {
                // color or texture
                if( xml.readNextStartElement() )
                {
                    material->setSpecularColor( colorOrTexture( xml ));
#ifdef DEBUG_MATERIALS
                    qDebug() << "set specular color to " << material->specularColor();
#endif
                }

                xml.skipCurrentElement();
                xml.readNextStartElement();
            }

            if( xml.name() == "shininess" )
            {
                if( xml.readNextStartElement() )
                {
                    float shininess = paramOrFloat( xml );
                    if( 0.0 < shininess && shininess < 1.0 )
                    {
                        qWarning() << "Warning: Blinn-Torrance-Sparrow not suported ( line" << xml.lineNumber()
                                << "), converting to Blinn-Phong";
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
            
            qDebug() << xml.tokenString() << xml.name();
            if( xml.name() == "transparent" )
            {
                // color or texture :/
                if( xml.readNextStartElement() )
                {
                    QColor transparent = colorOrTexture( xml );
                    Q_UNUSED( transparent );
#ifdef DEBUG_MATERIALS
                    qDebug() << "unused transparent color of " << transparent;
#endif
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }
            
            if( xml.name() == "transparency" )
            {
                if( xml.readNextStartElement() )
                {
                    float transparency = paramOrFloat( xml );
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
                    float indexOfRefraction = paramOrFloat( xml );
                    Q_UNUSED( indexOfRefraction );
                    qDebug() << "indexOfRefraction: "<<indexOfRefraction;
                    xml.skipCurrentElement();
                }
                xml.skipCurrentElement();
                xml.readNextStartElement();
            }
            xml.skipCurrentElement();
            xml.readNextStartElement();
        }
    }

    result->setMaterial( material );

    while( xml.name() == "pass" )
    {
        qWarning() << "Warning: pass element not supported ( line" << xml.lineNumber() << ")";
        xml.skipCurrentElement();
        xml.readNextStartElement();
    }

    // Make sure to exit cleanly
    findEndTag( xml, "technique" );
    return result;
}



QGLColladaParam* QGLColladaFXEffect::processNewparamElement( QXmlStreamReader& xml)
{
    qWarning() << "Warning: newparam element not supported ( line" << xml.lineNumber() << ")";
    xml.skipCurrentElement();
    return 0;
}
