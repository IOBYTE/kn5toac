#ifndef _KN5_H_
#define _KN5_H_

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <set>

class kn5
{
public:
    struct Matrix
    {
        float   m_data[4][4] =
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

        Matrix multiply(const Matrix& matrix) const;
    };

    struct Vec2 : public std::array<float, 2>
    {
    };

    struct Vec3 : public std::array<float, 3>
    {
        Vec3 transformPoint(const Matrix& matrix) const;
        Vec3 transformVector(const Matrix& matrix) const;
    };

    struct Vec4 : public std::array<float, 4>
    {
    };

    static int32_t readInt32(std::istream& stream);
    static float readFloat(std::istream& stream);
    static uint8_t readUint8(std::istream& stream);
    static uint16_t readUint16(std::istream& stream);
    static uint32_t readUint32(std::istream& stream);
    static bool readBool(std::istream& stream);
    static std::string readString(std::istream& stream, size_t length);
    static std::string readString(std::istream& stream);
    static Vec2 readVec2(std::istream& stream);
    static Vec3 readVec3(std::istream& stream);
    static Vec4 readVec4(std::istream& stream);

    struct Texture
    {
        int                 m_type = 0;
        std::string         m_name;
        std::vector<char>   m_data;

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
        std::string m_name;
        int32_t     m_slot = 0;
        std::string m_textureName;

        TextureMapping() = default;

        explicit TextureMapping(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    struct ShaderProperty
    {
        std::string m_name;
        float       m_value = 0;
        Vec2        m_value2 = { 0, 0 };
        Vec3        m_value3 = { 0, 0, 0 };
        Vec4        m_value4 = { 0, 0, 0, 0 };

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

        std::string                 m_name;
        std::string                 m_shaderName;
        AlphaBlendMode              m_alphaBlendMode = Opaque;
        bool                        m_alphaTested = false;
        DepthMode                   m_depthMode = DepthNormal;
        std::vector<ShaderProperty> m_shaderProperties;
        std::vector<TextureMapping> m_textureMappings;

        Material() = default;

        explicit Material(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
        const ShaderProperty* findShaderProperty(const std::string& name) const;
        const TextureMapping* findTextureMapping(const std::string& name) const;
    };

    struct Node
    {
        struct Vertex
        {
            Vec3    m_position = { 0, 0, 0 };
            Vec3    m_normal = { 0, 0, 0 };
            Vec2    m_texture = { 0 , 0 };
            Vec3    m_tangent = { 0, 0, 0 };

            Vertex() = default;
            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
            void transform(const Matrix& matrix);
        };

        struct BoundingSphere
        {
            Vec3    m_center = { 0, 0, 0 };
            float   m_radius = 0;

            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct Bone
        {
            std::string     m_name;
            Matrix          m_matrix;

            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct AnamatedVertex : public Vertex
        {
            Vec4    m_weights = { 0, 0, 0, 0 };
            Vec4    m_indices = { 0, 0, 0, 0 };

            AnamatedVertex() = default;
            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        enum NodeType : int32_t { NotSet, Transform, Mesh, SkinnedMesh };

        static std::string to_string(NodeType nodeType);

        NodeType                    m_type = NotSet;
        std::string                 m_name;
        bool                        m_active = false;
        Matrix                      m_matrix;
        bool                        m_castShadows = false;
        bool                        m_visible = false;
        bool                        m_transparent = false;
        std::vector<Bone>           m_bones;
        std::vector<Vertex>         m_vertices;
        std::vector<AnamatedVertex> m_anamatedVertices;
        std::vector<uint16_t>       m_indices;
        int                         m_materialID = 0;
        uint32_t                    m_layer = 0;
        float                       m_lodIn = 0;
        float                       m_lodOut = 0;
        BoundingSphere              m_boundingSphere;
        bool                        m_renderable = false;
        std::vector<Node>           m_children;
        Node                        * m_parent = nullptr;

        void read(std::istream& stream, Node* parent);
        void dump(std::ostream& stream, const std::string& indent = "") const;
        void readTranslation(std::istream& stream);
        void readMesh(std::istream& stream);
        void readAnimatedMesh(std::istream& stream);
        void transform(const Matrix& matrix);
        void removeEmptyNodes();
    };

    void readTextures(std::istream& stream);
    void readMaterials(std::istream& stream);

    void writeAc3dObject(std::ostream& fout, const Node& node, const std::set<int>& usedMaterialIDs, bool convertToPNG, bool outputACC, bool useDiffuse) const;
    void writeAc3dMaterials(std::ostream& fout, const Node& node, const std::set<int> & usedMaterialIDs) const;
    void getUsedMaterials(const kn5::Node& node, std::set<int>& usedMaterialIDs) const;
    int getNewMaterialID(int materialID, const std::set<int>& usedMaterialIDs) const;

    Node * kn5::findNode(Node& node, Node::NodeType type, const std::string& name);

    int32_t                 m_version = 0;
    int32_t                 m_unknown = 0;
    std::vector<Texture>    m_textures;
    std::vector<Material>   m_materials;
    Node                    m_node;

    kn5() = default;

    void read(const std::string& name);
    void dump(std::ostream& stream) const;
    void transform(const Matrix& matrix);
    void transform(Node &node, const Matrix& matrix);
    void removeEmptyNodes();
    Node * findNode(Node::NodeType type, const std::string& name);
    void writeTextures(const std::string& directory, bool convertToPNG) const;
    void writeAc3d(const std::string& file, bool convertToPNG, bool outputACC, bool useDiffuse) const;
    void writeAc3d(const std::string& file, const Node& node, bool convertToPNG, bool outputACC, bool useDiffuse) const;
};

#endif
