#ifndef _KN5_H_
#define _KN5_H_

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
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
        void makeIdentity();

        Matrix multiply(const Matrix& matrix) const;
    };

    struct Vec2 : public std::array<float, 2>
    {
    };

    struct Vec3 : public std::array<float, 3>
    {
        Vec3 transformPoint(const Matrix& matrix) const;
        Vec3 transformVector(const Matrix& matrix) const;

        Vec3 operator - (const Vec3& other) const
        {
            return Vec3{ at(0) - other.at(0),
                         at(1) - other.at(1),
                         at(2) - other.at(2) };
        }
        Vec3 cross(const Vec3& other) const
        {
            return Vec3{ at(1) * other.at(2) - at(2) * other.at(1),
                         at(2) * other.at(0) - at(0) * other.at(2),
                         at(0) * other.at(1) - at(1) * other.at(0) };
        }
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

    // Reads an int32 element count and checks it is non-negative and that
    // count * minElementSize bytes could still be left in the stream, so a
    // corrupt count throws instead of driving a huge resize().
    static size_t readCount(std::istream& stream, size_t minElementSize);
    // Bytes left between the current position and the end of the stream
    // (SIZE_MAX if the stream can't tell).
    static size_t bytesRemaining(std::istream& stream);

    // Deepest node nesting accepted, to stop a corrupt file recursing until the
    // stack overflows.
    static constexpr int MaxNodeDepth = 256;

    // True if name is a single plain file name that is safe to append to an
    // output directory: not empty, not "." or "..", and no path separators,
    // drive letters (':') or control characters. Names stored inside car files
    // (textures, data.acd entries, lods.ini) are checked with this before
    // anything is written, so a crafted file can't write outside the folder.
    static bool isPlainFileName(const std::string& name);

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
        TextureMapping* findTextureMapping(const std::string& name);
    };

    struct Node
    {
        struct Vertex
        {
            bool    m_animated = false;
            Vec3    m_position = { 0, 0, 0 };
            Vec3    m_normal = { 0, 0, 0 };
            Vec2    m_texture = { 0 , 0 };
            Vec3    m_tangent = { 0, 0, 0 };

            // animated  only
            Vec4    m_weights = { 0, 0, 0, 0 };
            Vec4    m_indices = { 0, 0, 0, 0 };

            Vertex() = default;
            void read(std::istream& stream, bool animated);
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
        std::vector<uint16_t>       m_indices;
        int                         m_materialID = 0;
        uint32_t                    m_layer = 0;
        float                       m_lodIn = 0;
        float                       m_lodOut = 0;
        BoundingSphere              m_boundingSphere;
        bool                        m_renderable = false;
        std::vector<Node>           m_children;
        Node                        * m_parent = nullptr;

        // Children are stored by value in a std::vector, so they move whenever the
        // vector reallocates or a sibling is erased, and a copied node gets new
        // children. These operations re-point each direct child's m_parent at its
        // new owner so m_parent never dangles. (Each child's own copy/move fixes
        // its children in turn.)
        //
        // A copy-constructed node is detached (m_parent = nullptr). Assignment
        // keeps the target's own m_parent, since the target stays where it is.
        Node() = default;
        Node(const Node& other)
        {
            assignFields(other);
        }
        Node(Node&& other) noexcept
            : m_parent(other.m_parent)
        {
            assignFields(std::move(other));
        }
        Node& operator=(const Node& other)
        {
            if (this != &other)
            {
                Node copy(other);   // copy first: other may live inside this subtree
                assignFields(std::move(copy));
            }
            return *this;
        }
        Node& operator=(Node&& other) noexcept
        {
            if (this != &other)
                assignFields(std::move(other));
            return *this;
        }
        ~Node() = default;

        void read(std::istream& stream, Node* parent, int depth = 0);
        // Throws if a mesh's material ID, index count or vertex indices are
        // out of range. Checked once after loading so later code can index
        // m_materials and m_vertices without bounds checks.
        void validate(size_t materialCount) const;
        void dump(std::ostream& stream, const std::string& indent = "") const;
        void dumpHierarchy(std::ostream& stream, const std::string& indent = "") const;
        void transform(const Matrix& matrix);
        bool removeChild(Node* child);
        void removeEmptyNodes();
        void removeInactiveNodes();
        Matrix getTransform() const;

    private:
        // Copies or moves every field except m_parent, then relinks the children.
        // Keep this in sync when adding fields to Node.
        template <typename N>
        void assignFields(N&& other)
        {
            m_type = other.m_type;
            m_name = std::forward<N>(other).m_name;
            m_active = other.m_active;
            m_matrix = other.m_matrix;
            m_castShadows = other.m_castShadows;
            m_visible = other.m_visible;
            m_transparent = other.m_transparent;
            m_bones = std::forward<N>(other).m_bones;
            m_vertices = std::forward<N>(other).m_vertices;
            m_indices = std::forward<N>(other).m_indices;
            m_materialID = other.m_materialID;
            m_layer = other.m_layer;
            m_lodIn = other.m_lodIn;
            m_lodOut = other.m_lodOut;
            m_boundingSphere = other.m_boundingSphere;
            m_renderable = other.m_renderable;
            m_children = std::forward<N>(other).m_children;

            for (auto& child : m_children)
                child.m_parent = this;
        }
    };

    void readTextures(std::istream& stream);
    void readMaterials(std::istream& stream);

    Node * findNode(Node& node, Node::NodeType type, const std::string& name);
    const Node* findNode(const Node& node, Node::NodeType type, const std::string& name) const;

    int32_t                 m_version = 0;
    int32_t                 m_unknown = 0;
    std::vector<Texture>    m_textures;
    std::vector<Material>   m_materials;
    Node                    m_node;

    kn5() = default;

    void read(const std::string& name);
    void dump(std::ostream& stream) const;
    void dumpHierarchy(std::ostream& stream) const;
    void transform(const Matrix& matrix);
    void transform(Node &node, const Matrix& matrix);
    void removeEmptyNodes();
    void removeInactiveNodes();
    Node* findNode(Node::NodeType type, const std::string& name);
    const Node* findNode(Node::NodeType type, const std::string& name) const;
};

#endif
