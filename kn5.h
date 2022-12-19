#ifndef _KN5_H_
#define _KN5_H_

#include <iostream>
#include <string>
#include <vector>
#include <array>

class kn5
{
private:
    static int32_t readInt32(std::istream& stream);
    static float readFloat(std::istream& stream);
    static uint8_t readUint8(std::istream& stream);
    static uint16_t readUint16(std::istream& stream);
    static uint32_t readUint32(std::istream& stream);
    static bool readBool(std::istream& stream);
    static std::string readString(std::istream& stream, size_t length);
    static std::string readString(std::istream& stream);

    struct Texture
    {
        int                 type = 0;
        std::string         name;
        std::vector<char>   data;

        Texture() = default;

        explicit Texture(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    struct TextureMapping
    {
        std::string name;
        int32_t     slot = 0;
        std::string textureName;

        TextureMapping() = default;

        explicit TextureMapping(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    struct Vec2 : public std::array<float, 2>
    {
    };

    struct Vec3 : public std::array<float, 3>
    {
    };

    struct Vec4 : public std::array<float, 4>
    {
    };

    struct ShaderProperty
    {
        std::string name;
        float       value = 0;
        Vec2        value2 = { 0, 0 };
        Vec3        value3 = { 0, 0, 0 };
        Vec4        value4 = { 0, 0, 0, 0 };

        ShaderProperty() = default;

        explicit ShaderProperty(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    struct Material
    {
        enum AlphaBlendMode : uint8_t { Opaque, AlphaBlend, AlphaToCoverage };

        static std::string to_string(AlphaBlendMode mode);

        enum DepthMode : int32_t { DepthNormal, DepthNoWrite, DepthOff };

        static std::string to_string(DepthMode mode);

        std::string                 name;
        std::string                 shaderName;
        AlphaBlendMode              alphaBlendMode = Opaque;
        bool                        alphaTested = false;
        DepthMode                   depthMode = DepthNormal;
        std::vector<ShaderProperty> shaderProperties;
        std::vector<TextureMapping> textureMappings;

        Material() = default;

        explicit Material(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void  dump(std::ostream& stream, const std::string& indent = "") const;
        const ShaderProperty* findShaderProperty(const std::string& name) const;
        const TextureMapping* findTextureMapping(const std::string& name) const;
    };

    struct Node
    {
        struct Vertex
        {
            Vec3    position = { 0, 0, 0 };
            Vec3    normal = { 0, 0, 0 };
            Vec3    texture = { 0 , 0 };
            Vec3    tangent = { 0, 0, 0 };

            Vertex() = default;
            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct BoundingSphere
        {
            Vec3    center = { 0, 0, 0 };
            float   radius = 0;

            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct Matrix
        {
            float   data[4][4] = 
            {
                 { 1.0f, 0.0f, 0.0f, 0.0f },
                 { 0.0f, 1.0f, 0.0f, 0.0f },
                 { 0.0f, 0.0f, 1.0f, 0.0f },
                 { 0.0f, 0.0f, 0.0f, 1.0f }
            };

            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
            bool isIdentity() const;
            bool isRotation() const;
            bool isTranslation() const;
        };

        struct Bone
        {
            std::string     name;
            Matrix          matrix;

            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct AnamatedVertex : public Vertex
        {
            Vec4    weights = { 0, 0, 0, 0 };
            Vec4    indices = { 0, 0, 0, 0 };

            AnamatedVertex() = default;
            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        enum NodeType : int32_t { NotSet, Transform, Mesh, SkinnedMesh };

        static std::string to_string(NodeType nodeType);

        NodeType                    type = NotSet;
        std::string                 name;
        bool                        active = false;
        Matrix                      matrix;
        bool                        castShadows = false;
        bool                        visible = false;
        bool                        transparent = false;
        std::vector<Bone>           bones;
        std::vector<Vertex>         vertices;
        std::vector<AnamatedVertex> anamatedVertices;
        std::vector<uint16_t>       indices;
        int                         materialID = 0;
        uint32_t                    layer = 0;
        float                       lodIn = 0;
        float                       lodOut = 0;
        BoundingSphere              boundingSphere;
        bool                        renderable = false;
        std::vector<Node>           children;

        Node() = default;

        explicit Node(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
        void readTranslation(std::istream& stream);
        void readMesh(std::istream& stream);
        void readAnimatedMesh(std::istream& stream);
    };

    void readTextures(std::istream& stream);
    void readMaterials(std::istream& stream);

    void writeAc3dObject(std::ostream& fout, const Node& node, bool convertToPNG, bool outputACC, bool useDiffuse) const;
    void writeAc3dMaterials(std::ostream& fout, const Node& node) const;

    int32_t                 version = 0;
    int32_t                 unknown = 0;
    std::vector<Texture>    textures;
    std::vector<Material>   materials;
    Node                    node;

public:
    kn5() = default;

    void read(const std::string& name);
    void dump(std::ostream& stream) const;
    void writeTextures(const std::string& directory, bool convertToPNG) const;
    void writeAc3d(const std::string& file, bool convertToPNG, bool outputACC, bool useDiffuse) const;
};

#endif
