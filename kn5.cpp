#include "kn5.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <limits>
#include <cmath>
#include <stdexcept>

int32_t kn5::readInt32(std::istream& stream)
{
    int32_t value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
    return value;
}

float kn5::readFloat(std::istream& stream)
{
    float value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(float));
    return value;
}

uint8_t kn5::readUint8(std::istream& stream)
{
    uint8_t value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
    return value;
}

uint16_t kn5::readUint16(std::istream& stream)
{
    uint16_t value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    return value;
}

uint32_t kn5::readUint32(std::istream& stream)
{
    uint32_t value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

bool kn5::readBool(std::istream& stream)
{
    // read the raw byte: storing anything other than 0 or 1 in a bool is undefined
    return readUint8(stream) != 0;
}

size_t kn5::bytesRemaining(std::istream& stream)
{
    const std::streampos position = stream.tellg();

    if (position == std::streampos(-1))
        return SIZE_MAX;

    stream.seekg(0, std::ios::end);
    const std::streampos end = stream.tellg();
    stream.seekg(position);

    if (end == std::streampos(-1) || end < position)
        return SIZE_MAX;

    return static_cast<size_t>(end - position);
}

bool kn5::isPlainFileName(const std::string& name)
{
    if (name.empty() || name == "." || name == "..")
        return false;

    for (const char c : name)
    {
        if (c == '/' || c == '\\' || c == ':' || static_cast<unsigned char>(c) < 0x20)
            return false;
    }

    return true;
}

size_t kn5::readCount(std::istream& stream, size_t minElementSize)
{
    const int32_t count = readInt32(stream);

    if (!stream)
        throw std::runtime_error("Unexpected end of file reading count");

    if (count < 0)
        throw std::runtime_error("Invalid count: " + std::to_string(count));

    if (minElementSize != 0 && static_cast<size_t>(count) > bytesRemaining(stream) / minElementSize)
        throw std::runtime_error("Count " + std::to_string(count) + " is larger than the rest of the file");

    return static_cast<size_t>(count);
}

std::string kn5::readString(std::istream& stream, size_t length)
{
    if (length > bytesRemaining(stream))
        throw std::runtime_error("String length " + std::to_string(length) + " is larger than the rest of the file");

    std::string string(length, '\0');
    stream.read(string.data(), static_cast<std::streamsize>(length));
    if (static_cast<size_t>(stream.gcount()) != length)
        throw std::runtime_error("Unexpected end of file reading string");
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
    const int32_t length = readInt32(stream);
    if (!stream || length < 0)
        throw std::runtime_error("Invalid string length: " + std::to_string(length));
    return readString(stream, static_cast<size_t>(length));
}

void kn5::Texture::read(std::istream& stream)
{
    m_type = readInt32(stream);
    m_name = readString(stream);
    m_data.resize(readCount(stream, 1));
    stream.read(m_data.data(), static_cast<std::streamsize>(m_data.size()));
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

    m_shaderProperties.resize(readCount(stream, 44));   // name + float + vec2 + vec3 + vec4

    for (auto& shaderProperty : m_shaderProperties)
        shaderProperty.read(stream);

    m_textureMappings.resize(readCount(stream, 12));    // name + slot + texture name

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

namespace
{
    // Rescales v to unit length (left alone if it has zero length).
    kn5::Vec3 normalized(const kn5::Vec3& v)
    {
        const float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

        if (length == 0.0f)
            return v;

        return kn5::Vec3{ v[0] / length, v[1] / length, v[2] / length };
    }
}

void kn5::Node::Vertex::transform(const Matrix& matrix)
{
    m_position = m_position.transformPoint(matrix);
    // a matrix with scaling changes the length of normals and tangents; keep them unit length
    m_normal = normalized(m_normal.transformVector(matrix));
    m_tangent = normalized(m_tangent.transformVector(matrix));
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

void kn5::Node::read(std::istream& stream, Node* parent, int depth)
{
    if (depth > MaxNodeDepth)
        throw std::runtime_error("Node hierarchy nested deeper than " + std::to_string(MaxNodeDepth) + " levels");

    m_parent = parent;
    m_type = static_cast<NodeType>(readInt32(stream));
    m_name = readString(stream);

    m_children.resize(readCount(stream, 13));          // smallest possible node

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
            m_bones.resize(readCount(stream, 68));         // name + matrix

            for (auto& bone : m_bones)
                bone.read(stream);
        }

        m_vertices.resize(readCount(stream, m_type == SkinnedMesh ? 76 : 44));

        for (auto& vertex : m_vertices)
            vertex.read(stream, m_type == SkinnedMesh);

        m_indices.resize(readCount(stream, 2));

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
        child.read(stream, this, depth + 1);
}

void kn5::Node::validate(size_t materialCount) const
{
    if (m_type == Mesh || m_type == SkinnedMesh)
    {
        if (m_materialID < 0 || static_cast<size_t>(m_materialID) >= materialCount)
            throw std::runtime_error("Mesh " + m_name + ": material ID " + std::to_string(m_materialID) +
                                     " out of range (" + std::to_string(materialCount) + " materials)");

        if (m_indices.size() % 3 != 0)
            throw std::runtime_error("Mesh " + m_name + ": index count " + std::to_string(m_indices.size()) +
                                     " is not a multiple of 3");

        for (const auto index : m_indices)
        {
            if (index >= m_vertices.size())
                throw std::runtime_error("Mesh " + m_name + ": vertex index " + std::to_string(index) +
                                         " out of range (" + std::to_string(m_vertices.size()) + " vertices)");
        }
    }

    for (const auto& child : m_children)
        child.validate(materialCount);
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

// Removes transform nodes with no children anywhere below this node.
// Children are cleaned first, so a group that only contained empty groups is
// removed as well.
void kn5::Node::removeEmptyNodes()
{
    for (auto& child : m_children)
        child.removeEmptyNodes();

    for (std::vector<Node>::iterator it = m_children.begin(); it != m_children.end(); )
    {
        if (it->m_type == Transform && it->m_children.empty())
            it = m_children.erase(it);
        else
            it++;
    }
}

// Removes inactive transform nodes (and everything under them) anywhere below
// this node. Inactive nodes are removed before recursing, so their subtrees
// are never visited.
void kn5::Node::removeInactiveNodes()
{
    for (std::vector<Node>::iterator it = m_children.begin(); it != m_children.end(); )
    {
        if (it->m_type == Transform && it->m_active == false)
            it = m_children.erase(it);
        else
            it++;
    }

    for (auto& child : m_children)
        child.removeInactiveNodes();
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
    m_textures.resize(readCount(stream, 12));     // type + name + size

    for (auto& texture : m_textures)
        texture.read(stream);
}

void kn5::readMaterials(std::istream& stream)
{
    m_materials.resize(readCount(stream, 22));    // smallest possible material

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

    m_node.validate(m_materials.size());
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
    // An empty name almost always means a missing ini value (getValue returns
    // ""). Matching it would pick whichever mesh happens to have no name, so
    // treat it as "not found".
    if (name.empty())
        return nullptr;

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
    // see the non-const overload: an empty name never matches
    if (name.empty())
        return nullptr;

    return findNode(m_node, type, name);
}
