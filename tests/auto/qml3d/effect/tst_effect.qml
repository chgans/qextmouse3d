import Qt 4.7
import Qt3D 1.0
import Qt3D.Test 1.0

Viewport {
    id: viewport

    Item3D {
        Effect {
            id: "effect"
        }
    }

    Material {
        id: "china"
        ambientColor: "#c09680"
        specularColor: "#3c3c3c"
        shininess: 128
    }

    TestCase {
        name: "Effect"

        function test_color() {
            verify(effect.color == "#ffffff", "Default color is white");
            effect.color = "#ff4488";
            verify(effect.color == "#ff4488", "setColor()")
        }

        function test_lighting() {
            verify(effect.useLighting == true, "Default useLighting value is true");
            effect.useLighting = false;
            verify(effect.useLighting == false, "setUseLighting()")
        }

        function test_decal() {
            verify(effect.decal == false, "Default decal");
            effect.decal = true;
            verify(effect.decal == true, "setDecal()");
        }

        function test_blending() {
            verify(effect.blending == false, "Default blending");
            effect.blending = true;
            verify(effect.blending == true, "setBlending()");
        }

        function test_texture() {
            verify(effect.texture == "", "Default texture");
            effect.texture = "test.png";
            verify(effect.texture != "");
            var textureString = ("" + effect.texture);
            var protocolString = textureString.substring(0,5);
            var stringLength = textureString.length;
            var fileNameFragment = textureString.substring(stringLength-8,stringLength);
            compare(protocolString, "file:", "recognize local file");
            compare(fileNameFragment, "test.png", "correct file name");
        }

        function test_textureImage() {
            // No support for interrogating QImages from QML
            // so can't write meaningful test case for textureImage
        }

        function test_material() {
            verify(!effect.material, "default material is null")
            effect.material = china;
            compare(effect.material, china, "setMaterial()");
        }

        function test_progress() {
            verify(effect.progress == 0.0);
        }
    }
}
