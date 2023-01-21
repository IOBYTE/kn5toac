#include "kn5.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <limits>

int32_t kn5::readInt32(std::istream& stream)
{
    int32_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
    return value;
}

float kn5::readFloat(std::istream& stream)
{
    float value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(float));
    return value;
}

uint8_t kn5::readUint8(std::istream& stream)
{
    uint8_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
    return value;
}

uint16_t kn5::readUint16(std::istream& stream)
{
    uint16_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    return value;
}

uint32_t kn5::readUint32(std::istream& stream)
{
    uint32_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

bool kn5::readBool(std::istream& stream)
{
    bool value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(bool));
    return value;
}

std::string kn5::readString(std::istream& stream, size_t length)
{
    char* text = new char[length + 1];
    text[length] = 0;
    stream.read(text, length);
    std::string string(text, length);
    delete[] text;
    return string;
}

kn5::Vec2 kn5::readVec2(std::istream& stream)
{
    kn5::Vec2   vec;
    vec[0] = readFloat(stream);
    vec[1] = readFloat(stream);
    return vec;
}

kn5::Vec3 kn5::readVec3(std::istream& stream)
{
    kn5::Vec3   vec;
    vec[0] = readFloat(stream);
    vec[1] = readFloat(stream);
    vec[2] = readFloat(stream);
    return vec;
}

kn5::Vec4 kn5::readVec4(std::istream& stream)
{
    kn5::Vec4   vec;
    vec[0] = readFloat(stream);
    vec[1] = readFloat(stream);
    vec[2] = readFloat(stream);
    vec[3] = readFloat(stream);
    return vec;
}

kn5::Vec3 kn5::Vec3::transformPoint(const Matrix& matrix) const
{
    Vec3 dst;

    float t0 = at(0);
    float t1 = at(1);
    float t2 = at(2);

    dst[0] = t0 * matrix.m_data[0][0] + t1 * matrix.m_data[1][0] + t2 * matrix.m_data[2][0] + matrix.m_data[3][0];
    dst[1] = t0 * matrix.m_data[0][1] + t1 * matrix.m_data[1][1] + t2 * matrix.m_data[2][1] + matrix.m_data[3][1];
    dst[2] = t0 * matrix.m_data[0][2] + t1 * matrix.m_data[1][2] + t2 * matrix.m_data[2][2] + matrix.m_data[3][2];

    return dst;
}

kn5::Vec3 kn5::Vec3::transformVector(const Matrix& matrix) const
{
    Vec3 dst;

    float t0 = at(0);
    float t1 = at(1);
    float t2 = at(2);

    dst[0] = t0 * matrix.m_data[0][0] + t1 * matrix.m_data[1][0] + t2 * matrix.m_data[2][0];
    dst[1] = t0 * matrix.m_data[0][1] + t1 * matrix.m_data[1][1] + t2 * matrix.m_data[2][1];
    dst[2] = t0 * matrix.m_data[0][2] + t1 * matrix.m_data[1][2] + t2 * matrix.m_data[2][2];

    return dst;
}

kn5::Matrix kn5::Matrix::multiply(const Matrix & matrix) const
{
    Matrix dst;

    for (int j = 0; j < 4; j++)
    {
        dst.m_data[0][j] = matrix.m_data[0][0] * (*this).m_data[0][j] +
            matrix.m_data[0][1] * (*this).m_data[1][j] +
            matrix.m_data[0][2] * (*this).m_data[2][j] +
            matrix.m_data[0][3] * (*this).m_data[3][j];

        dst.m_data[1][j] = matrix.m_data[1][0] * (*this).m_data[0][j] +
            matrix.m_data[1][1] * (*this).m_data[1][j] +
            matrix.m_data[1][2] * (*this).m_data[2][j] +
            matrix.m_data[1][3] * (*this).m_data[3][j];

        dst.m_data[2][j] = matrix.m_data[2][0] * (*this).m_data[0][j] +
            matrix.m_data[2][1] * (*this).m_data[1][j] +
            matrix.m_data[2][2] * (*this).m_data[2][j] +
            matrix.m_data[2][3] * (*this).m_data[3][j];

        dst.m_data[3][j] = matrix.m_data[3][0] * (*this).m_data[0][j] +
            matrix.m_data[3][1] * (*this).m_data[1][j] +
            matrix.m_data[3][2] * (*this).m_data[2][j] +
            matrix.m_data[3][3] * (*this).m_data[3][j];
    }

    return dst;
}

void kn5::Matrix::makeIdentity()
{
    *this = Matrix();
}

std::string kn5::readString(std::istream& stream)
{
    return readString(stream, readInt32(stream));
}

void kn5::Texture::read(std::istream& stream)
{
    m_type = readInt32(stream);
    m_name = readString(stream);
    const int size = readInt32(stream);
    m_data.resize(size);
    stream.read(m_data.data(), size);
}

void kn5::Texture::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "type: " << m_type << std::endl;
    stream << indent << "name: " << m_name << std::endl;
    stream << indent << "size: " << m_data.size() << std::endl;
}

void kn5::TextureMapping::read(std::istream& stream)
{
    m_name = readString(stream);
    m_slot = readInt32(stream);
    m_textureName = readString(stream);
}

void kn5::TextureMapping::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:        " << m_name << std::endl;
    stream << indent << "slot:        " << m_slot << std::endl;
    stream << indent << "textureName: " << m_textureName << std::endl;
}

void kn5::ShaderProperty::read(std::istream& stream)
{
    m_name = readString(stream);
    m_value = readFloat(stream);
    m_value2 = readVec2(stream);
    m_value3 = readVec3(stream);
    m_value4 = readVec4(stream);
}

void kn5::ShaderProperty::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:   " << m_name << std::endl;
    stream << indent << "value:  " << m_value << std::endl;
    stream << indent << "value2: " << m_value2[0] << ", " << m_value2[1] << std::endl;
    stream << indent << "value3: " << m_value3[0] << ", " << m_value3[1] << ", " << m_value3[2] << std::endl;
    stream << indent << "value5: " << m_value4[0] << ", " << m_value4[1] << ", " << m_value4[2] << ", " << m_value4[3] << std::endl;
}

void kn5::Material::read(std::istream& stream)
{
    m_name = readString(stream);
    m_shaderName = readString(stream);
    m_alphaBlendMode = static_cast<AlphaBlendMode>(readUint8(stream));
    m_alphaTested = readBool(stream);
    m_depthMode = static_cast<DepthMode>(readInt32(stream));

    m_shaderProperties.resize(readInt32(stream));

    for (auto& shaderProperty : m_shaderProperties)
        shaderProperty.read(stream);

    m_textureMappings.resize(readInt32(stream));

    for (auto& textureMapping : m_textureMappings)
        textureMapping.read(stream);
}

std::string kn5::Material::to_string(AlphaBlendMode mode)
{
    if (mode == Opaque)
        return "Opaque";

    if (mode == AlphaBlend)
        return "AlphaBlend";

    if (mode == AlphaToCoverage)
        return "AlphaToCoverage";

    return "Unknown(" + std::to_string(mode) + ")";
}

std::string kn5::Material::to_string(DepthMode mode)
{
    if (mode == DepthNormal)
        return "DepthNormal";

    if (mode == DepthNoWrite)
        return "DepthNoWrite";

    if (mode == DepthOff)
        return "DepthOff";

    return "Unknown(" + std::to_string(mode) + ")";
}

void kn5::Material::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:             " << m_name << std::endl;
    stream << indent << "shaderName:       " << m_shaderName << std::endl;
    stream << indent << "alphaBlendMode:   " << kn5::Material::to_string(m_alphaBlendMode) << std::endl;
    stream << indent << "alphaTested:      " << (m_alphaTested ? "true" : "false") << std::endl;
    stream << indent << "depthMode:        " << kn5::Material::to_string(m_depthMode) << std::endl;

    stream << indent << "shaderProperties: " << m_shaderProperties.size() << std::endl;
    for (size_t i = 0; i < m_shaderProperties.size(); i++)
    {
        stream << indent << "shaderProperties[" << i << "]" << std::endl;
        m_shaderProperties[i].dump(stream, indent + "  ");
    }

    stream << indent << "textureMappings:  " << m_textureMappings.size() << std::endl;
    for (size_t i = 0; i < m_textureMappings.size(); i++)
    {
        stream << indent << "textureMappings[" << i << "]" << std::endl;
        m_textureMappings[i].dump(stream, indent + "  ");
    }
}

const kn5::TextureMapping* kn5::Material::findTextureMapping(const std::string& name) const
{
    for (size_t i = 0; i < m_textureMappings.size(); i++)
    {
        if (m_textureMappings[i].m_name == name)
            return &m_textureMappings[i];
    }

    return nullptr;
}

kn5::TextureMapping* kn5::Material::findTextureMapping(const std::string& name)
{
    for (size_t i = 0; i < m_textureMappings.size(); i++)
    {
        if (m_textureMappings[i].m_name == name)
            return &m_textureMappings[i];
    }

    return nullptr;
}

const kn5::ShaderProperty* kn5::Material::findShaderProperty(const std::string& name) const
{
    for (size_t i = 0; i < m_shaderProperties.size(); i++)
    {
        if (m_shaderProperties[i].m_name == name)
            return &m_shaderProperties[i];
    }

    return nullptr;
}

std::string kn5::Node::to_string(NodeType nodeType)
{
    if (nodeType == Transform)
        return "Transform";

    if (nodeType == Mesh)
        return "Mesh";

    if (nodeType == SkinnedMesh)
        return "SkinnedMesh";

    if (nodeType == NotSet)
        return "NotSet";

    return "Unknown(" + std::to_string(nodeType) + ")";
}

void kn5::Node::Vertex::read(std::istream& stream, bool animated)
{
    m_animated = animated;
    m_position = readVec3(stream);
    m_normal = readVec3(stream);
    m_texture = readVec2(stream);
    m_tangent = readVec3(stream);

    if (m_animated)
    {
        m_weights = readVec4(stream);
        m_indices = readVec4(stream);
    }
}

void kn5::Node::Vertex::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "position: " << m_position[0] << ", " << m_position[1] << ", " << m_position[2] << std::endl;
    stream << indent << "normal:   " << m_normal[0] << ", " << m_normal[1] << ", " << m_normal[2] << std::endl;
    stream << indent << "texture:  " << m_texture[0] << ", " << m_texture[1] << std::endl;
    stream << indent << "tangent:  " << m_tangent[0] << ", " << m_tangent[1] << ", " << m_tangent[2] << std::endl;

    if (m_animated)
    {
        stream << indent << "weights:  " << m_weights[0] << ", " << m_weights[1] << ", " << m_weights[2] << ", " << m_weights[3] << std::endl;
        stream << indent << "indices:  " << m_indices[0] << ", " << m_indices[1] << ", " << m_indices[2] << ", " << m_indices[3] << std::endl;
    }
}

void kn5::Node::Vertex::transform(const Matrix& matrix)
{
    m_position = m_position.transformPoint(matrix);
    m_normal = m_normal.transformVector(matrix);
    m_tangent = m_tangent.transformVector(matrix);
}

void kn5::Node::BoundingSphere::read(std::istream& stream)
{
    m_center = readVec3(stream);
    m_radius = readFloat(stream);
}

void kn5::Node::BoundingSphere::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "center: " << m_center[0] << ", " << m_center[1] << ", " << m_center[2] << std::endl;
    stream << indent << "radius:  " << m_radius << std::endl;
}

void kn5::Matrix::read(std::istream& stream)
{
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            m_data[i][j] = readFloat(stream);
        }
    }
}

void kn5::Matrix::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "matrix:" << std::endl;
    stream << indent << "  " << m_data[0][0] << ", " << m_data[0][1] << ", " << m_data[0][2] << ", " << m_data[0][3] << std::endl;
    stream << indent << "  " << m_data[1][0] << ", " << m_data[1][1] << ", " << m_data[1][2] << ", " << m_data[1][3] << std::endl;
    stream << indent << "  " << m_data[2][0] << ", " << m_data[2][1] << ", " << m_data[2][2] << ", " << m_data[2][3] << std::endl;
    stream << indent << "  " << m_data[3][0] << ", " << m_data[3][1] << ", " << m_data[3][2] << ", " << m_data[3][3] << std::endl;
}

bool kn5::Matrix::isIdentity() const
{
    return m_data[0][0] == 1 && m_data[0][1] == 0 && m_data[0][2] == 0 && m_data[0][3] == 0 &&
           m_data[1][0] == 0 && m_data[1][1] == 1 && m_data[1][2] == 0 && m_data[1][3] == 0 &&
           m_data[2][0] == 0 && m_data[2][1] == 0 && m_data[2][2] == 1 && m_data[2][3] == 0 &&
           m_data[3][0] == 0 && m_data[3][1] == 0 && m_data[3][2] == 0 && m_data[3][3] == 1;
}

bool kn5::Matrix::isRotation() const
{
    return m_data[0][0] != 1 || m_data[0][1] != 0 || m_data[0][2] != 0 ||
           m_data[1][0] != 0 || m_data[1][1] != 1 || m_data[1][2] != 0 ||
           m_data[2][0] != 0 || m_data[2][1] != 0 || m_data[2][2] != 1;
}

bool kn5::Matrix::isTranslation() const
{
    return m_data[3][0] != 0 || m_data[3][1] != 0 || m_data[3][2] != 0;
}

void kn5::Node::Bone::read(std::istream& stream)
{
    m_name = readString(stream);
    m_matrix.read(stream);
}

void kn5::Node::Bone::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:  " << m_name << std::endl;
    m_matrix.dump(stream, indent);
}

void kn5::Node::read(std::istream& stream, Node* parent)
{
    m_parent = parent;
    m_type = static_cast<NodeType>(readInt32(stream));
    m_name = readString(stream);

    m_children.resize(readInt32(stream));

    m_active = readBool(stream);

    if (m_type == Transform)
    {
        m_matrix.read(stream);
    }
    else
    {
        m_castShadows = readBool(stream);
        m_visible = readBool(stream);
        m_transparent = readBool(stream);

        if (m_type == SkinnedMesh)
        {
            m_bones.resize(readInt32(stream));

            for (auto& bone : m_bones)
                bone.read(stream);
        }

        m_vertices.resize(readInt32(stream));

        for (auto& vertex : m_vertices)
            vertex.read(stream, m_type == SkinnedMesh);

        m_indices.resize(readInt32(stream));

        for (auto& index : m_indices)
            index = readUint16(stream);

        m_materialID = readInt32(stream);
        m_layer = readUint32(stream);
        m_lodIn = readFloat(stream);
        m_lodOut = readFloat(stream);

        if (m_type == Mesh)
        {
            m_boundingSphere.read(stream);
            m_renderable = readBool(stream);
        }
    }

    for (auto & child : m_children)
        child.read(stream, this);
}

void kn5::Node::transform(const Matrix& matrix)
{
    if (m_type != Transform)
    {
        for (auto& vertex : m_vertices)
            vertex.transform(matrix);
    }
    else
    {
        const Matrix newXform = matrix.multiply(m_matrix);

        m_matrix = Matrix();

        for (auto& child : m_children)
            child.transform(newXform);
    }
}

kn5::Matrix kn5::Node::getTransform() const
{
    kn5::Matrix matrix = m_matrix;
    kn5::Node* parent = m_parent;

    while (parent)
    {
        matrix = parent->m_matrix.multiply(matrix);
        parent = parent->m_parent;
    }

    return matrix;
}

bool kn5::Node::removeChild(Node* child)
{
    for (std::vector<Node>::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        if (&(*it) == child)
        {
            m_children.erase(it);

            return true;
        }
    }

    return false;
}

void kn5::Node::removeEmptyNodes()
{
    for (std::vector<Node>::iterator it = m_children.begin(); it != m_children.end(); )
    {
        if (it->m_type == Transform && it->m_children.empty())
            it = m_children.erase(it);
        else
            it++;
    }
}

void kn5::Node::removeInactiveNodes()
{
    for (std::vector<Node>::iterator it = m_children.begin(); it != m_children.end(); )
    {
        if (it->m_type == Transform && it->m_active == false)
            it = m_children.erase(it);
        else
            it++;
    }
}

void kn5::Node::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "type:        " << kn5::Node::to_string(m_type) << std::endl;
    stream << indent << "name:        " << m_name << std::endl;
    stream << indent << "active:      " << (m_active ? "true" : "false") << std::endl;

    if (m_type == Transform)
        m_matrix.dump(stream, indent);
    else
    {
        stream << indent << "castShadows: " << (m_castShadows ? "true" : "false") << std::endl;
        stream << indent << "visible:     " << (m_visible ? "true" : "false") << std::endl;
        stream << indent << "transparent: " << (m_transparent ? "true" : "false") << std::endl;

        if (m_type == SkinnedMesh)
        {
            stream << indent << "bones:       " << m_bones.size() << std::endl;
            for (size_t i = 0; i < m_bones.size(); i++)
            {
                stream << indent << "bones[" << i << "]" << std::endl;
                m_bones[i].dump(stream, indent + "  ");
            }
        }

        stream << indent << "vertices:    " << m_vertices.size() << std::endl;
        for (size_t i = 0; i < m_vertices.size(); i++)
        {
            stream << indent << "vertices[" << i << "]" << std::endl;
            m_vertices[i].dump(stream, indent + "  ");
        }

        stream << indent << "indices:    " << m_indices.size() << std::endl;
        for (size_t i = 0; i < m_indices.size(); i++)
            stream << indent << "  indices[" << i << "]: " << m_indices[i] << std::endl;

        stream << indent << "materialID: " << m_materialID << std::endl;
        stream << indent << "layer:      " << m_layer << std::endl;
        stream << indent << "lodIn:      " << m_lodIn << std::endl;
        stream << indent << "lodOut:     " << m_lodOut << std::endl;

        if (m_type == Mesh)
        {
            stream << indent << "boundingSphere:" << std::endl;
            m_boundingSphere.dump(stream, indent + "  ");
            stream << indent << "renderable: " << (m_renderable ? "true" : "false") << std::endl;

        }
    }

    stream << indent << "children: " << m_children.size() << std::endl;
    for (size_t i = 0; i < m_children.size(); i++)
    {
        stream << indent << "children[" << i << "]" << std::endl;
        m_children[i].dump(stream, indent + "  ");
    }
}

void kn5::Node::dumpHierarchy(std::ostream& stream, const std::string& indent) const
{
    stream << indent << m_name;

    if (m_type == Node::Transform)
    {
        if (m_matrix.isIdentity())
            stream << "  Null";
        else if (m_matrix.isTranslation() && m_matrix.isRotation())
            stream << "  Translation Rotation";
        else if (m_matrix.isTranslation())
            stream << "  Translation";
        else if (m_matrix.isRotation())
            stream << "  Rotation";
    }
    else if (m_type == Node::Mesh)
        stream << "  Mesh";
    else
        stream << "  SkinnedMesh";

    stream << (m_active ? "  Active" : "  NotActive");

    if (m_type == Node::Transform)
        stream << "  " << m_children.size() << " children";

    stream << std::endl;

    for (size_t i = 0; i < m_children.size(); i++)
    {
        m_children[i].dumpHierarchy(stream, indent + "  ");
    }
}

void kn5::readTextures(std::istream& stream)
{
    m_textures.resize(readInt32(stream));

    for (auto& texture : m_textures)
        texture.read(stream);
}

void kn5::readMaterials(std::istream& stream)
{
    m_materials.resize(readInt32(stream));

    for (auto& material : m_materials)
        material.read(stream);
}

void kn5::read(const std::string& name)
{
    std::ifstream   stream;

    stream.open(name, std::ios::binary);

    if (!stream)
        throw std::runtime_error("Couldn't open file");

    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (readString(stream, 6) != "sc6969")
        throw std::runtime_error("Not a valid kn5 file");

    m_version = readInt32(stream);

    if (m_version > 5)
        m_unknown = readInt32(stream);

    readTextures(stream);

    readMaterials(stream);

    m_node.read(stream, nullptr);
}

void kn5::dump(std::ostream& stream) const
{
    stream << "version: " << m_version << std::endl;

    if (m_version > 5)
        stream << "unknown: " << m_unknown << std::endl;

    stream << "textures: " << m_textures.size() << std::endl;
    for (size_t i = 0; i < m_textures.size(); i++)
    {
        stream << "textures[" << i << "]" << std::endl;
        m_textures[i].dump(stream, "  ");
    }

    stream << "materials: " << m_materials.size() << std::endl;
    for (size_t i = 0; i < m_materials.size(); i++)
    {
        stream << "materials[" << i << "]" << std::endl;
        m_materials[i].dump(stream, "  ");
    }

    stream << "node:" << std::endl;
    m_node.dump(stream, "  ");
}

void kn5::dumpHierarchy(std::ostream& stream) const
{
    m_node.dumpHierarchy(stream);
}


void kn5::transform(const Matrix& matrix)
{
    m_node.transform(matrix);
}

void kn5::transform(Node &node, const Matrix& matrix)
{
    node.transform(matrix);
}

void kn5::removeEmptyNodes()
{
    m_node.removeEmptyNodes();
}

void kn5::removeInactiveNodes()
{
    m_node.removeInactiveNodes();
}

kn5::Node* kn5::findNode(Node &node, Node::NodeType type, const std::string& name)
{
    if (node.m_type == type && node.m_name == name)
        return &node;

    for (auto& child : node.m_children)
    {
        Node* found = findNode(child, type, name);

        if (found)
            return found;
    }

    return nullptr;
}

kn5::Node* kn5::findNode(Node::NodeType type, const std::string& name)
{
    return findNode(m_node, type, name);
}

const kn5::Node* kn5::findNode(const Node& node, Node::NodeType type, const std::string& name) const
{
    if (node.m_type == type && node.m_name == name)
        return &node;

    for (const auto& child : node.m_children)
    {
        const Node* found = findNode(child, type, name);

        if (found)
            return found;
    }

    return nullptr;
}

const kn5::Node* kn5::findNode(Node::NodeType type, const std::string& name) const
{
    return findNode(m_node, type, name);
}
