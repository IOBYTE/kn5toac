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

std::string kn5::readString(std::istream& stream)
{
    return readString(stream, readInt32(stream));
}

void kn5::Texture::read(std::istream& stream)
{
    type = readInt32(stream);
    name = readString(stream);
    const int size = readInt32(stream);
    data.resize(size);
    stream.read(data.data(), size);
}

void kn5::Texture::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "type: " << type << std::endl;
    stream << indent << "name: " << name << std::endl;
    stream << indent << "size: " << data.size() << std::endl;
}


void kn5::TextureMapping::read(std::istream& stream)
{
    name = readString(stream);
    slot = readInt32(stream);
    textureName = readString(stream);
}

void kn5::TextureMapping::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:        " << name << std::endl;
    stream << indent << "slot:        " << slot << std::endl;
    stream << indent << "textureName: " << textureName << std::endl;
}

void kn5::ShaderProperty::read(std::istream& stream)
{
    name = readString(stream);
    value = readFloat(stream);
    value2[0] = readFloat(stream);
    value2[1] = readFloat(stream);
    value3[0] = readFloat(stream);
    value3[1] = readFloat(stream);
    value3[2] = readFloat(stream);
    value4[0] = readFloat(stream);
    value4[1] = readFloat(stream);
    value4[2] = readFloat(stream);
    value4[3] = readFloat(stream);
}

void kn5::ShaderProperty::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:   " << name << std::endl;
    stream << indent << "value:  " << value << std::endl;
    stream << indent << "value2: " << value2[0] << ", " << value2[1] << std::endl;
    stream << indent << "value3: " << value3[0] << ", " << value3[1] << ", " << value3[2] << std::endl;
    stream << indent << "value5: " << value4[0] << ", " << value4[1] << ", " << value4[2] << ", " << value4[3] << std::endl;
}

void kn5::Material::read(std::istream& stream)
{
    name = readString(stream);
    shaderName = readString(stream);
    alphaBlendMode = static_cast<AlphaBlendMode>(readUint8(stream));
    alphaTested = readBool(stream);
    depthMode = static_cast<DepthMode>(readInt32(stream));

    shaderProperties.resize(readInt32(stream));

    for (auto& shaderProperty : shaderProperties)
        shaderProperty.read(stream);

    textureMappings.resize(readInt32(stream));

    for (auto& textureMapping : textureMappings)
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
    stream << indent << "name:             " << name << std::endl;
    stream << indent << "shaderName:       " << shaderName << std::endl;
    stream << indent << "alphaBlendMode:   " << kn5::Material::to_string(alphaBlendMode) << std::endl;
    stream << indent << "alphaTested:      " << (alphaTested ? "true" : "false") << std::endl;
    stream << indent << "depthMode:        " << kn5::Material::to_string(depthMode) << std::endl;

    stream << indent << "shaderProperties: " << shaderProperties.size() << std::endl;
    for (size_t i = 0; i < shaderProperties.size(); i++)
    {
        stream << indent << "shaderProperties[" << i << "]" << std::endl;
        shaderProperties[i].dump(stream, indent + "  ");
    }

    stream << indent << "textureMappings:  " << textureMappings.size() << std::endl;
    for (size_t i = 0; i < textureMappings.size(); i++)
    {
        stream << indent << "textureMappings[" << i << "]" << std::endl;
        textureMappings[i].dump(stream, indent + "  ");
    }
}

const kn5::TextureMapping* kn5::Material::findTextureMapping(const std::string& name) const
{
    for (size_t i = 0; i < textureMappings.size(); i++)
    {
        if (textureMappings[i].name == name)
            return &textureMappings[i];
    }

    return nullptr;
}

const kn5::ShaderProperty* kn5::Material::findShaderProperty(const std::string& name) const
{
    for (size_t i = 0; i < shaderProperties.size(); i++)
    {
        if (shaderProperties[i].name == name)
            return &shaderProperties[i];
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
    position[0] = readFloat(stream);
    position[1] = readFloat(stream);
    position[2] = readFloat(stream);
    normal[0] = readFloat(stream);
    normal[1] = readFloat(stream);
    normal[2] = readFloat(stream);
    texture[0] = readFloat(stream);
    texture[1] = readFloat(stream);
    tangent[0] = readFloat(stream);
    tangent[1] = readFloat(stream);
    tangent[2] = readFloat(stream);
}

void kn5::Node::Vertex::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "position: " << position[0] << ", " << position[1] << ", " << position[2] << std::endl;
    stream << indent << "normal:   " << normal[0] << ", " << normal[1] << ", " << normal[2] << std::endl;
    stream << indent << "texture:  " << texture[0] << ", " << texture[1] << std::endl;
    stream << indent << "tangent:  " << tangent[0] << ", " << tangent[1] << ", " << tangent[2] << std::endl;
}

void kn5::Node::BoundingSphere::read(std::istream& stream)
{
    center[0] = readFloat(stream);
    center[1] = readFloat(stream);
    center[2] = readFloat(stream);
    radius = readFloat(stream);
}

void kn5::Node::BoundingSphere::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "center: " << center[0] << ", " << center[1] << ", " << center[2] << std::endl;
    stream << indent << "radius:  " << radius << std::endl;
}

void kn5::Node::Matrix::read(std::istream& stream)
{
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            data[i][j] = readFloat(stream);
        }
    }
}

void kn5::Node::Matrix::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "matrix:" << std::endl;
    stream << indent << "  " << data[0][0] << ", " << data[0][1] << ", " << data[0][2] << ", " << data[0][3] << std::endl;
    stream << indent << "  " << data[1][0] << ", " << data[1][1] << ", " << data[1][2] << ", " << data[1][3] << std::endl;
    stream << indent << "  " << data[2][0] << ", " << data[2][1] << ", " << data[2][2] << ", " << data[2][3] << std::endl;
    stream << indent << "  " << data[3][0] << ", " << data[3][1] << ", " << data[3][2] << ", " << data[3][3] << std::endl;
}

bool kn5::Node::Matrix::isIdentity() const
{
    return data[0][0] == 1 && data[0][1] == 0 && data[0][2] == 0 && data[0][3] == 0 &&
           data[1][0] == 0 && data[1][1] == 1 && data[1][2] == 0 && data[1][3] == 0 &&
           data[2][0] == 0 && data[2][1] == 0 && data[2][2] == 1 && data[2][3] == 0 &&
           data[3][0] == 0 && data[3][1] == 0 && data[3][2] == 0 && data[3][3] == 1;
}

bool kn5::Node::Matrix::isRotation() const
{
    return data[0][0] != 1 || data[0][1] != 0 || data[0][2] != 0 ||
           data[1][0] != 0 || data[1][1] != 1 || data[1][2] != 0 ||
           data[2][0] != 0 || data[2][1] != 0 || data[2][2] != 1;
}

bool kn5::Node::Matrix::isTranslation() const
{
    return data[3][0] != 0 || data[3][1] != 0 || data[3][2] != 0;
}

void kn5::Node::Bone::read(std::istream& stream)
{
    name = readString(stream);
    matrix.read(stream);
}

void kn5::Node::Bone::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "name:  " << name << std::endl;
    matrix.dump(stream, indent);
}

void kn5::Node::AnamatedVertex::read(std::istream& stream)
{
    Vertex::read(stream);

    weights[0] = readFloat(stream);
    weights[1] = readFloat(stream);
    weights[2] = readFloat(stream);
    weights[3] = readFloat(stream);

    indices[0] = readFloat(stream);
    indices[1] = readFloat(stream);
    indices[2] = readFloat(stream);
    indices[3] = readFloat(stream);
}

void kn5::Node::AnamatedVertex::dump(std::ostream& stream, const std::string& indent) const
{
    Vertex::dump(stream, indent);

    stream << indent << "weights:  " << weights[0] << ", " << weights[1] << ", " << weights[2] << ", " << weights[3] << std::endl;
    stream << indent << "indices:  " << indices[0] << ", " << indices[1] << ", " << indices[2] << ", " << indices[3] << std::endl;
}

void kn5::Node::read(std::istream& stream)
{
    type = static_cast<NodeType>(readInt32(stream));
    name = readString(stream);

    const int32_t childCount = readInt32(stream);

    active = readBool(stream);

    switch (type)
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

    for (int i = 0; i < childCount; i++)
        children.emplace_back(stream);
}

void kn5::Node::readTranslation(std::istream& stream)
{
    matrix.read(stream);
}

void kn5::Node::readMesh(std::istream& stream)
{
    castShadows = readBool(stream);
    visible = readBool(stream);
    transparent = readBool(stream);

    vertices.resize(readInt32(stream));

    for (auto & vertex : vertices)
        vertex.read(stream);

    indices.resize(readInt32(stream));

    for (auto & index : indices)
        index = readUint16(stream);

    materialID = readInt32(stream);
    layer = readUint32(stream);
    lodIn = readFloat(stream);
    lodOut = readFloat(stream);
    boundingSphere.read(stream);
    renderable = readBool(stream);
}

void kn5::Node::readAnimatedMesh(std::istream& stream)
{
    castShadows = readBool(stream);
    visible = readBool(stream);
    transparent = readBool(stream);

    bones.resize(readInt32(stream));

    for (auto& bone : bones)
        bone.read(stream);

    anamatedVertices.resize(readInt32(stream));

    for (auto& vertex : anamatedVertices)
        vertex.read(stream);

    indices.resize(readInt32(stream));

    for (auto& index : indices)
        index = readUint16(stream);

    materialID = readInt32(stream);
    layer = readUint32(stream);
    lodIn = readFloat(stream);
    lodOut = readFloat(stream);
}

void kn5::Node::dump(std::ostream& stream, const std::string& indent) const
{
    stream << indent << "type:        " << kn5::Node::to_string(type) << std::endl;
    stream << indent << "name:        " << name << std::endl;
    stream << indent << "active:      " << (active ? "true" : "false") << std::endl;

    switch (type)
    {
    case 1:
        matrix.dump(stream, indent);
        break;
    case 2:
        stream << indent << "castShadows: " << (castShadows ? "true" : "false") << std::endl;
        stream << indent << "visible:     " << (visible ? "true" : "false") << std::endl;
        stream << indent << "transparent: " << (transparent ? "true" : "false") << std::endl;

        stream << indent << "vertices:    " << vertices.size() << std::endl;
        for (size_t i = 0; i < vertices.size(); i++)
        {
            stream << indent << "vertices[" << i << "]" << std::endl;
            vertices[i].dump(stream, indent + "  ");
        }

        stream << indent << "indices:    " << indices.size() << std::endl;
        for (size_t i = 0; i < indices.size(); i++)
            stream << indent << "  indices[" << i << "]: " << indices[i] << std::endl;

        stream << indent << "materialID: " << materialID << std::endl;
        stream << indent << "layer:      " << layer << std::endl;
        stream << indent << "lodIn:      " << lodIn << std::endl;
        stream << indent << "lodOut:     " << lodOut << std::endl;
        stream << indent << "boundingSphere:" << std::endl;
        boundingSphere.dump(stream, indent + "  ");
        stream << indent << "renderable: " << (renderable ? "true" : "false") << std::endl;
        break;
    case 3:
        stream << indent << "castShadows: " << (castShadows ? "true" : "false") << std::endl;
        stream << indent << "visible:     " << (visible ? "true" : "false") << std::endl;
        stream << indent << "transparent: " << (transparent ? "true" : "false") << std::endl;

        stream << indent << "bones:       " << bones.size() << std::endl;
        for (size_t i = 0; i < bones.size(); i++)
        {
            stream << indent << "bones[" << i << "]" << std::endl;
            bones[i].dump(stream, indent + "  ");
        }

        stream << indent << "anamatedVertices:    " << anamatedVertices.size() << std::endl;
        for (size_t i = 0; i < anamatedVertices.size(); i++)
        {
            stream << indent << "anamatedVertices[" << i << "]" << std::endl;
            anamatedVertices[i].dump(stream, indent + "  ");
        }

        stream << indent << "indices:    " << indices.size() << std::endl;
        for (size_t i = 0; i < indices.size(); i++)
            stream << indent << "  indices[" << i << "]: " << indices[i] << std::endl;

        stream << indent << "materialID: " << materialID << std::endl;
        stream << indent << "layer:      " << layer << std::endl;
        stream << indent << "lodIn:      " << lodIn << std::endl;
        stream << indent << "lodOut:     " << lodOut << std::endl;
        break;
    }

    stream << indent << "children: " << children.size() << std::endl;
    for (size_t i = 0; i < children.size(); i++)
    {
        stream << indent << "children[" << i << "]" << std::endl;
        children[i].dump(stream, indent + "  ");
    }
}

void kn5::readTextures(std::istream& stream)
{
    textures.resize(readInt32(stream));

    for (auto& texture : textures)
        texture.read(stream);
}

void kn5::writeTextures(const std::string& directory, bool convertToPNG) const
{
    if (!std::filesystem::exists(directory))
    {
        if (!std::filesystem::create_directory(directory))
            throw std::runtime_error("Couldn't create directory: " + directory);
    }

    for (size_t i = 0; i < textures.size(); i++)
    {
        std::filesystem::path texturePath(directory);

        texturePath.append(textures[i].name);

        std::string texture = texturePath.string();

        if (!std::filesystem::exists(texturePath))
        {
            std::ofstream   fout(texturePath.string(), std::ios::binary);

            if (!fout)
                throw std::runtime_error("Couldn't create texture: " + texture);

            fout.write(textures[i].data.data(), textures[i].data.size());

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
                std::string command("magick convert " + texture + " ");

                for (const auto & material : materials)
                {
                    if (material.textureMappings[0].textureName == textures[i].name)
                    {
                        if (material.alphaBlendMode == Material::Opaque)
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
}

void kn5::readMaterials(std::istream& stream)
{
    materials.resize(readInt32(stream));

    for (auto& material : materials)
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

    version = readInt32(stream);

    if (version > 5)
        unknown = readInt32(stream);

    readTextures(stream);

    readMaterials(stream);

    node.read(stream);
}

void kn5::dump(std::ostream& stream) const
{
    stream << "version: " << version << std::endl;

    stream << "textures: " << textures.size() << std::endl;
    for (size_t i = 0; i < textures.size(); i++)
    {
        stream << "textures[" << i << "]" << std::endl;
        textures[i].dump(stream, "  ");
    }

    stream << "materials: " << materials.size() << std::endl;
    for (size_t i = 0; i < materials.size(); i++)
    {
        stream << "materials[" << i << "]" << std::endl;
        materials[i].dump(stream, "  ");
    }

    stream << "node:" << std::endl;
    node.dump(stream, "  ");
}

void kn5::writeAc3d(const std::string& file, bool convertToPNG, bool outputACC, bool useDiffuse) const
{
    std::ofstream fout(file);

    if (fout)
    {
        fout << "AC3Db" << std::endl;

        writeAc3dMaterials(fout, node);

        fout << "OBJECT world" << std::endl;
        fout << "kids 1" << std::endl;
        fout << "OBJECT group" << std::endl;
        fout << "rot " << 0 << " " << 0 << " " << -1;
        fout << " " << 0 << " " << 1 << " " << 0;
        fout << " " << 1 << " " << 0 << " " << 0 << std::endl;
        fout << "kids 1" << std::endl;

        writeAc3dObject(fout, node, convertToPNG, outputACC, useDiffuse);

        fout.close();
    }
}

void kn5::writeAc3dMaterials(std::ostream& fout, const Node& node) const
{
    for (const auto& material : materials)
    {
        fout << "MATERIAL " << "\"" << material.name << "\"";

        const ShaderProperty* property = material.findShaderProperty("ksDiffuse");

        if (property != nullptr)
        {
            const float rgb = std::clamp(property->value, 0.0f, 1.0f);
            fout << " rgb " << rgb << " " << rgb << " " << rgb;
        }
        else
            fout << " rgb 1 1 1";

        property = material.findShaderProperty("ksAmbient");

        if (property != nullptr)
        {
            const float amb = std::clamp(property->value, 0.0f, 1.0f);
            fout << "  amb " << amb << " " << amb << " " << amb;
        }
        else
            fout << "  amb 1 1 1";

        property = material.findShaderProperty("ksEmissive");

        if (property != nullptr)
        {
            const float emis = std::clamp(property->value, 0.0f, 1.0f);
            fout << "  emis " << emis << " " << emis << " " << emis;
        }
        else
            fout << "  emis 1 1 1";

        property = material.findShaderProperty("ksSpecular");

        if (property != nullptr)
        {
            const float spec = std::clamp(property->value, 0.0f, 1.0f);
            fout << "  spec " << spec << " " << spec << " " << spec;
        }
        else
            fout << "  spec 1 1 1";

        property = material.findShaderProperty("ksSpecularEXP");  // FIXME is this the right parameter?

        if (property != nullptr)
        {
            // FIXME should this be scaled?
            const float shi = std::clamp(property->value, 0.0f, 128.0f);
            fout << "  shi " << static_cast<int>(shi);
        }
        else
            fout << "  shi 0";

        property = material.findShaderProperty("ksAlphaRef");  // FIXME is this the right parameter?

        if (property != nullptr)
        {
            const float trans = std::clamp(property->value, 0.0f, 1.0f);
            fout << "  trans " << trans;
        }
        else
            fout << "  trans 0";

        fout << std::endl;
    }
}

void kn5::writeAc3dObject(std::ostream& fout, const kn5::Node& node, bool convertToPNG, bool outputACC, bool useDiffuse) const
{
    if (node.type == Node::Transform)
    {
        fout << "OBJECT group" << std::endl;
        fout << "name \"" << node.name << "\"" << std::endl;

        if (node.matrix.isRotation())
        {
            fout << "rot " << node.matrix.data[0][0] << " " << node.matrix.data[0][1] << " " << node.matrix.data[0][2];
            fout << " " << node.matrix.data[1][0] << " " << node.matrix.data[1][1] << " " << node.matrix.data[1][2];
            fout << " " << node.matrix.data[2][0] << " " << node.matrix.data[2][1] << " " << node.matrix.data[2][2] << std::endl;
        }

        if (node.matrix.isTranslation())
            fout << "loc " << node.matrix.data[3][0] << " " << node.matrix.data[3][1] << " " << node.matrix.data[3][2] << std::endl;
    }
    else if (node.type == Node::Mesh || node.type == Node::SkinnedMesh)
    {
        fout << "OBJECT poly" << std::endl;
        fout << "name \"" << node.name << "\"" << std::endl;

        std::string texture;
        const TextureMapping* txDiffuse = materials[node.materialID].findTextureMapping("txDiffuse");

        if (useDiffuse)
            texture = txDiffuse->textureName;
        else
        {
            const ShaderProperty* useDetail = materials[node.materialID].findShaderProperty("useDetail");
            const TextureMapping* txDetail = materials[node.materialID].findTextureMapping("txDetail");

            texture = ((useDetail && useDetail != 0) && txDetail) ? txDetail->textureName : txDiffuse->textureName;
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

        fout << "numvert " << node.vertices.size() << std::endl;

        for (const auto& vertex : node.vertices)
        {
            fout << vertex.position[0] << " " << vertex.position[1] << " " << vertex.position[2];

            if (outputACC)
                fout << " " << vertex.normal[0] << " " << vertex.normal[1] << " " << vertex.normal[2] << std::endl;

            fout << std::endl;
        }

        float uvMult = 1.0f;

        if (useDiffuse)
        {
            const ShaderProperty* property = materials[node.materialID].findShaderProperty("diffuseMult");

            if (property != nullptr)
                uvMult = property->value;
        }
        else
        {
            const ShaderProperty* property = materials[node.materialID].findShaderProperty("useDetail");

            if (property == nullptr || property->value == 0.0f)
            {
                property = materials[node.materialID].findShaderProperty("diffuseMult");

                if (property != nullptr)
                    uvMult = property->value;
            }
            else
            {
                property = materials[node.materialID].findShaderProperty("detailUVMultiplier");

                if (property != nullptr)
                    uvMult = 1 / property->value;
            }
        }

        fout << "numsurf " << (node.indices.size() / 3) << std::endl;
        for (size_t i = 0; i < node.indices.size(); i += 3)
        {
            fout << "SURF 0x10" << std::endl;
            fout << "mat " << node.materialID << std::endl;
            fout << "refs 3" << std::endl;
            for (size_t j = 0; j < 3; j++)
            {
                const int index = node.indices[i + j];
                if (node.type == Node::Mesh)
                    fout << index << " " << (node.vertices[index].texture[0] * uvMult) << " " << (-node.vertices[index].texture[1] * uvMult) << std::endl;
                else
                    fout << index << " " << (node.anamatedVertices[index].texture[0] * uvMult) << " " << (-node.anamatedVertices[index].texture[1] * uvMult) << std::endl;
            }
        }
    }
    else
        return;

    fout << "kids " << node.children.size() << std::endl;

    for (const auto& child : node.children)
        writeAc3dObject(fout, child, convertToPNG, outputACC, useDiffuse);
}
