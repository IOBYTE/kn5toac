#ifndef _KN5_H_
#define _KN5_H_

#include <iostream>
#include <string>
#include <vector>

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

    struct Sample
    {
        std::string name;
        int32_t     slot = 0;
        std::string textureName;

        Sample() = default;

        explicit Sample(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    struct Property
    {
        std::string name;
        float       value = 0;
        float       value2[2] = { 0, 0 };
        float       value3[3] = { 0, 0, 0 };
        float       value4[4] = { 0, 0, 0, 0 };

        Property() = default;

        explicit Property(std::istream& stream)
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

        std::string             name;
        std::string             shaderName;
        AlphaBlendMode          alphaBlendMode = Opaque;
        bool                    alphaTested = false;
        DepthMode               depthMode = DepthNormal;
        std::vector<Property>   properties;
        std::vector<Sample>     samples;

        Material() = default;

        explicit Material(std::istream& stream)
        {
            read(stream);
        }

        void read(std::istream& stream);
        void  dump(std::ostream& stream, const std::string& indent = "") const;
        const Property* findProperty(const std::string& name) const;
        const Sample* findSample(const std::string& name) const;
    };

    struct Node
    {
        struct Vertex
        {
            float   position[3] = { 0, 0, 0 };
            float   normal[3] = { 0, 0, 0 };
            float   texture[2] = { 0 , 0 };
            float   tangent[3] = { 0, 0, 0 };

            Vertex() = default;
            void read(std::istream& stream);
            void dump(std::ostream& stream, const std::string& indent = "") const;
        };

        struct BoundingSphere
        {
            float   center[3] = { 0, 0, 0 };
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
            float   weights[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

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
