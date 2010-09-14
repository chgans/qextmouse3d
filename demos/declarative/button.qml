import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480

    Item3d {
        id: button_mesh
        name: "button_mesh"
        mesh: Mesh {
            source: "meshes/button3d.3ds"
            options: "ShowWarnings ForceSmooth"
        }
        cullFaces: "CullBackFaces"

        Item3d {
            id: top_node
            objectName: "top_node"
            name: "top_node"
            meshNode: "Top"
            effect: Effect { material: face_texture }
        }

        Material {
            id: face_texture
            objectName: "face_texture"
            ambientColor: "#000000"
            specularColor: "#000000"
            shininess: 16
            textureUrl: "textures/button/face.png"
        }

        Item3d {
            id: side_plate_node
            objectName: "side_plate_node"
            name: "side_plate_node"
            meshNode: "SidePlate"
            //effect: Effect { material: address_texture }
        }

        Material {
            id: address_texture
            objectName: "address_texture"
            ambientColor: "#000000"
            specularColor: "#000000"
            shininess: 16
            textureUrl: "textures/button/full-address.png"
        }

        Light {
            id: l1
            position: Qt.vector3d(-1, -10, 10)
        }
    }
}
