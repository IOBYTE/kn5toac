#include "kn5.h"

#include <fstream>
#include <filesystem>
#include <algorithm>

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

void kn5::Node::Vertex::read(std::istream& stream)
{
    m_position = readVec3(stream);
    m_normal = readVec3(stream);
    m_texture = readVec2(stream);
    m_tangent = readVec3(stream);
}

void kn5::Node::Vertex::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "position: " << m_position[0] << ", " << m_position[1] << ", " << m_position[2] << std::endl;
    stream << indent << "normal:   " << m_normal[0] << ", " << m_normal[1] << ", " << m_normal[2] << std::endl;
    stream << indent << "texture:  " << m_texture[0] << ", " << m_texture[1] << std::endl;
    stream << indent << "tangent:  " << m_tangent[0] << ", " << m_tangent[1] << ", " << m_tangent[2] << std::endl;
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

void kn5::Node::AnamatedVertex::read(std::istream& stream)
{
    Vertex::read(stream);

    m_weights = readVec4(stream);
    m_indices = readVec4(stream);
}

void kn5::Node::AnamatedVertex::dump(std::ostream& stream, const std::string& indent) const
{
    Vertex::dump(stream, indent);

    stream << indent << "weights:  " << m_weights[0] << ", " << m_weights[1] << ", " << m_weights[2] << ", " << m_weights[3] << std::endl;
    stream << indent << "indices:  " << m_indices[0] << ", " << m_indices[1] << ", " << m_indices[2] << ", " << m_indices[3] << std::endl;
}

void kn5::Node::read(std::istream& stream, Node* parent)
{
    m_parent = parent;
    m_type = static_cast<NodeType>(readInt32(stream));
    m_name = readString(stream);

    m_children.resize(readInt32(stream));

    m_active = readBool(stream);

    switch (m_type)
    {
    case Transform:
        readTranslation(stream);
        break;
    case Mesh:
        readMesh(stream);
        break;
    case SkinnedMesh:
        readAnimatedMesh(stream);
        break;
    }

    for (auto & child : m_children)
        child.read(stream, this);
}

void kn5::Node::readTranslation(std::istream& stream)
{
    m_matrix.read(stream);
}

void kn5::Node::readMesh(std::istream& stream)
{
    m_castShadows = readBool(stream);
    m_visible = readBool(stream);
    m_transparent = readBool(stream);

    m_vertices.resize(readInt32(stream));

    for (auto & vertex : m_vertices)
        vertex.read(stream);

    m_indices.resize(readInt32(stream));

    for (auto & index : m_indices)
        index = readUint16(stream);

    m_materialID = readInt32(stream);
    m_layer = readUint32(stream);
    m_lodIn = readFloat(stream);
    m_lodOut = readFloat(stream);
    m_boundingSphere.read(stream);
    m_renderable = readBool(stream);
}

void kn5::Node::readAnimatedMesh(std::istream& stream)
{
    m_castShadows = readBool(stream);
    m_visible = readBool(stream);
    m_transparent = readBool(stream);

    m_bones.resize(readInt32(stream));

    for (auto& bone : m_bones)
        bone.read(stream);

    m_anamatedVertices.resize(readInt32(stream));

    for (auto& vertex : m_anamatedVertices)
        vertex.read(stream);

    m_indices.resize(readInt32(stream));

    for (auto& index : m_indices)
        index = readUint16(stream);

    m_materialID = readInt32(stream);
    m_layer = readUint32(stream);
    m_lodIn = readFloat(stream);
    m_lodOut = readFloat(stream);
}

void kn5::Node::transform(const Matrix& matrix)
{
    if (m_type != Transform)
    {
        if (m_type == Mesh)
        {
            for (auto& vertex : m_vertices)
                vertex.transform(matrix);
        }
        else
        {
            for (auto& vertex : m_anamatedVertices)
                vertex.transform(matrix);
        }
    }
    else
    {
        const Matrix newXform = matrix.multiply(m_matrix);

        m_matrix = Matrix();

        for (auto& child : m_children)
            child.transform(newXform);
    }
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

void kn5::Node::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "type:        " << kn5::Node::to_string(m_type) << std::endl;
    stream << indent << "name:        " << m_name << std::endl;
    stream << indent << "active:      " << (m_active ? "true" : "false") << std::endl;

    switch (m_type)
    {
    case 1:
        m_matrix.dump(stream, indent);
        break;
    case 2:
        stream << indent << "castShadows: " << (m_castShadows ? "true" : "false") << std::endl;
        stream << indent << "visible:     " << (m_visible ? "true" : "false") << std::endl;
        stream << indent << "transparent: " << (m_transparent ? "true" : "false") << std::endl;

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
        stream << indent << "boundingSphere:" << std::endl;
        m_boundingSphere.dump(stream, indent + "  ");
        stream << indent << "renderable: " << (m_renderable ? "true" : "false") << std::endl;
        break;
    case 3:
        stream << indent << "castShadows: " << (m_castShadows ? "true" : "false") << std::endl;
        stream << indent << "visible:     " << (m_visible ? "true" : "false") << std::endl;
        stream << indent << "transparent: " << (m_transparent ? "true" : "false") << std::endl;

        stream << indent << "bones:       " << m_bones.size() << std::endl;
        for (size_t i = 0; i < m_bones.size(); i++)
        {
            stream << indent << "bones[" << i << "]" << std::endl;
            m_bones[i].dump(stream, indent + "  ");
        }

        stream << indent << "anamatedVertices:    " << m_anamatedVertices.size() << std::endl;
        for (size_t i = 0; i < m_anamatedVertices.size(); i++)
        {
            stream << indent << "anamatedVertices[" << i << "]" << std::endl;
            m_anamatedVertices[i].dump(stream, indent + "  ");
        }

        stream << indent << "indices:    " << m_indices.size() << std::endl;
        for (size_t i = 0; i < m_indices.size(); i++)
            stream << indent << "  indices[" << i << "]: " << m_indices[i] << std::endl;

        stream << indent << "materialID: " << m_materialID << std::endl;
        stream << indent << "layer:      " << m_layer << std::endl;
        stream << indent << "lodIn:      " << m_lodIn << std::endl;
        stream << indent << "lodOut:     " << m_lodOut << std::endl;
        break;
    }

    stream << indent << "children: " << m_children.size() << std::endl;
    for (size_t i = 0; i < m_children.size(); i++)
    {
        stream << indent << "children[" << i << "]" << std::endl;
        m_children[i].dump(stream, indent + "  ");
    }
}

void kn5::readTextures(std::istream& stream)
{
    m_textures.resize(readInt32(stream));

    for (auto& texture : m_textures)
        texture.read(stream);
}

void kn5::writeTextures(const std::string& directory, bool convertToPNG, bool deleteDDS) const
{
    if (!std::filesystem::exists(directory))
    {
        if (!std::filesystem::create_directory(directory))
            throw std::runtime_error("Couldn't create directory: " + directory);
    }

    std::set<std::string>   filesToDelete;

    for (size_t i = 0; i < m_textures.size(); i++)
    {
        std::filesystem::path texturePath(directory);

        texturePath.append(m_textures[i].m_name);

        const std::string texture = texturePath.string();

        if (!std::filesystem::exists(texturePath))
        {
            std::ofstream   fout(texturePath.string(), std::ios::binary);

            if (!fout)
                throw std::runtime_error("Couldn't create texture: " + texture);

            fout.write(m_textures[i].m_data.data(), m_textures[i].m_data.size());

            fout.close();
        }

        if (convertToPNG && (texture.find(".png") == std::string::npos && texture.find(".PNG") == std::string::npos))
        {
            std::string png;

            size_t extension = texture.find(".dds");

            if (extension != std::string::npos)
                png = texture.substr(0, extension) + ".png";
            else if ((extension = texture.find(".DDS")) != std::string::npos)
                png = texture.substr(0, extension) + ".png";

            if (!png.empty() && !std::filesystem::exists(png))
            {
                filesToDelete.insert(texture);

                std::string command("magick convert " + texture + " ");

                for (const auto & material : m_materials)
                {
                    if (material.m_textureMappings[0].m_textureName == m_textures[i].m_name)
                    {
                        if (material.m_alphaBlendMode == Material::Opaque)
                        {
                            command += "-alpha off ";

                            break;
                        }
                    }
                }

                command += png;

                if (system(command.c_str()) == -1)
                    std::cerr << "failed to convert " << texture << " to " << png << std::endl;
            }
        }
    }

    for (const auto& file : filesToDelete)
        std::filesystem::remove(file);
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

void kn5::writeAc3d(const std::string& file, bool convertToPNG, bool outputACC, bool useDiffuse) const
{
    writeAc3d(file, m_node, convertToPNG, outputACC, useDiffuse);
}

void kn5::writeAc3d(const std::string& file, const Node& node, bool convertToPNG, bool outputACC, bool useDiffuse) const
{
    std::set<int>   usedMaterialIDs;

    getUsedMaterials(node, usedMaterialIDs);

    std::ofstream fout(file);

    if (fout)
    {
        fout << "AC3Db" << std::endl;

        writeAc3dMaterials(fout, node, usedMaterialIDs);

        fout << "OBJECT world" << std::endl;
        fout << "kids 1" << std::endl;

        writeAc3dObject(fout, node, usedMaterialIDs, convertToPNG, outputACC, useDiffuse);

        fout.close();
    }
}

void kn5::getUsedMaterials(const kn5::Node& node, std::set<int>& usedMaterialIDs) const
{
    if (node.m_type != kn5::Node::Transform)
        usedMaterialIDs.insert(node.m_materialID);

    for (const auto& child : node.m_children)
        getUsedMaterials(child, usedMaterialIDs);
}

int kn5::getNewMaterialID(int materialID, const std::set<int>& usedMaterialIDs) const
{
    int newMaterialID = 0;

    for (auto id : usedMaterialIDs)
    {
        if (id == materialID)
            return newMaterialID;

        newMaterialID++;
    }
    return 0;
}

void kn5::writeAc3dMaterials(std::ostream& fout, const Node& node, const std::set<int>& usedMaterialIDs) const
{
    for (auto materialID : usedMaterialIDs)
    {
        const kn5::Material& material = m_materials[materialID];

        fout << "MATERIAL " << "\"" << material.m_name << "\"";

        const ShaderProperty* property = material.findShaderProperty("ksDiffuse");

        if (property != nullptr)
        {
            const float rgb = std::clamp(property->m_value, 0.0f, 1.0f);
            fout << " rgb " << rgb << " " << rgb << " " << rgb;
        }
        else
            fout << " rgb 1 1 1";

        property = material.findShaderProperty("ksAmbient");

        if (property != nullptr)
        {
            const float amb = std::clamp(property->m_value, 0.0f, 1.0f);
            fout << "  amb " << amb << " " << amb << " " << amb;
        }
        else
            fout << "  amb 1 1 1";

        property = material.findShaderProperty("ksEmissive");

        if (property != nullptr)
        {
            const float emis = std::clamp(property->m_value, 0.0f, 1.0f);
            fout << "  emis " << emis << " " << emis << " " << emis;
        }
        else
            fout << "  emis 1 1 1";

        property = material.findShaderProperty("ksSpecular");

        if (property != nullptr)
        {
            const float spec = std::clamp(property->m_value, 0.0f, 1.0f);
            fout << "  spec " << spec << " " << spec << " " << spec;
        }
        else
            fout << "  spec 1 1 1";

        property = material.findShaderProperty("ksSpecularEXP");  // FIXME is this the right parameter?

        if (property != nullptr)
        {
            // FIXME should this be scaled?
            const float shi = std::clamp(property->m_value, 0.0f, 128.0f);
            fout << "  shi " << static_cast<int>(shi);
        }
        else
            fout << "  shi 0";

        property = material.findShaderProperty("ksAlphaRef");  // FIXME is this the right parameter?

        if (property != nullptr)
        {
            const float trans = std::clamp(property->m_value, 0.0f, 1.0f);
            fout << "  trans " << trans;
        }
        else
            fout << "  trans 0";

        fout << std::endl;
    }
}

void kn5::writeAc3dObject(std::ostream& fout, const kn5::Node& node, const std::set<int>& usedMaterialIDs, bool convertToPNG, bool outputACC, bool useDiffuse) const
{
    if (node.m_type == Node::Transform)
    {
        fout << "OBJECT group" << std::endl;
        fout << "name \"" << node.m_name << "\"" << std::endl;

        if (node.m_matrix.isRotation())
        {
            fout << "rot " << node.m_matrix.m_data[0][0] << " " << node.m_matrix.m_data[0][1] << " " << node.m_matrix.m_data[0][2];
            fout << " " << node.m_matrix.m_data[1][0] << " " << node.m_matrix.m_data[1][1] << " " << node.m_matrix.m_data[1][2];
            fout << " " << node.m_matrix.m_data[2][0] << " " << node.m_matrix.m_data[2][1] << " " << node.m_matrix.m_data[2][2] << std::endl;
        }

        if (node.m_matrix.isTranslation())
            fout << "loc " << node.m_matrix.m_data[3][0] << " " << node.m_matrix.m_data[3][1] << " " << node.m_matrix.m_data[3][2] << std::endl;
    }
    else if (node.m_type == Node::Mesh || node.m_type == Node::SkinnedMesh)
    {
        fout << "OBJECT poly" << std::endl;
        fout << "name \"" << node.m_name << "\"" << std::endl;

        std::string texture;
        const TextureMapping* txDiffuse = m_materials[node.m_materialID].findTextureMapping("txDiffuse");

        if (useDiffuse)
            texture = txDiffuse->m_textureName;
        else
        {
            const ShaderProperty* useDetail = m_materials[node.m_materialID].findShaderProperty("useDetail");
            const TextureMapping* txDetail = m_materials[node.m_materialID].findTextureMapping("txDetail");

            texture = ((useDetail && useDetail != 0) && txDetail) ? txDetail->m_textureName : txDiffuse->m_textureName;
        }

        if (convertToPNG && (texture.find(".png") == std::string::npos && texture.find(".PNG") == std::string::npos))
        {
            const std::string png = texture;

            size_t extension = texture.find(".dds");

            if (extension != std::string::npos)
                texture = texture.substr(0, extension) + ".png";
            else if ((extension = png.find(".DDS")) != std::string::npos)
                texture = texture.substr(0, extension) + ".png";
        }

        if (outputACC)
        {
            fout << "texture \"" << texture << "\" base" << std::endl;
            fout << "texture empty_texture_no_mapping tiled" << std::endl;
            fout << "texture empty_texture_no_mapping skids" << std::endl;
            fout << "texture empty_texture_no_mapping shad" << std::endl;
        }
        else
            fout << "texture \"" << texture << "\"" << std::endl;

        fout << "numvert " << node.m_vertices.size() << std::endl;

        for (const auto& vertex : node.m_vertices)
        {
            fout << vertex.m_position[0] << " " << vertex.m_position[1] << " " << vertex.m_position[2];

            if (outputACC)
                fout << " " << vertex.m_normal[0] << " " << vertex.m_normal[1] << " " << vertex.m_normal[2];

            fout << std::endl;
        }

        float uvMult = 1.0f;

        if (useDiffuse)
        {
            const ShaderProperty* property = m_materials[node.m_materialID].findShaderProperty("diffuseMult");

            if (property != nullptr)
                uvMult = property->m_value;
        }
        else
        {
            const ShaderProperty* property = m_materials[node.m_materialID].findShaderProperty("useDetail");

            if (property == nullptr || property->m_value == 0.0f)
            {
                property = m_materials[node.m_materialID].findShaderProperty("diffuseMult");

                if (property != nullptr)
                    uvMult = property->m_value;
            }
            else
            {
                property = m_materials[node.m_materialID].findShaderProperty("detailUVMultiplier");

                if (property != nullptr)
                    uvMult = 1 / property->m_value;
            }
        }

        fout << "numsurf " << (node.m_indices.size() / 3) << std::endl;
        for (size_t i = 0; i < node.m_indices.size(); i += 3)
        {
            fout << "SURF 0x10" << std::endl;
            fout << "mat " << getNewMaterialID(node.m_materialID, usedMaterialIDs) << std::endl;
            fout << "refs 3" << std::endl;
            for (size_t j = 0; j < 3; j++)
            {
                const int index = node.m_indices[i + j];
                if (node.m_type == Node::Mesh)
                    fout << index << " " << (node.m_vertices[index].m_texture[0] * uvMult) << " " << (-node.m_vertices[index].m_texture[1] * uvMult) << std::endl;
                else
                    fout << index << " " << (node.m_anamatedVertices[index].m_texture[0] * uvMult) << " " << (-node.m_anamatedVertices[index].m_texture[1] * uvMult) << std::endl;
            }
        }
    }
    else
        return;

    fout << "kids " << node.m_children.size() << std::endl;

    for (const auto& child : node.m_children)
        writeAc3dObject(fout, child, usedMaterialIDs, convertToPNG, outputACC, useDiffuse);
}
