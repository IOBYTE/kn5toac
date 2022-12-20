#include "kn5.h"
#include "ini.h"

#include <fstream>
#include <filesystem>

void extract(kn5& model, const std::string& name, const kn5::Matrix& xform, const std::string & file, bool wheels)
{
    kn5::Node* transformNode = model.findNode(kn5::Node::Transform, name);

    if (transformNode)
    {
        kn5::Node   node = wheels ? *transformNode : transformNode->m_children[0];

        node.transform(xform);

        model.writeAc3d(file, node, true, file.find(".acc") != std::string::npos, true);

        if (transformNode->m_parent)
        {
            for (std::vector<kn5::Node>::iterator it = transformNode->m_parent->m_children.begin(); it != transformNode->m_parent->m_children.end(); ++it)
            {
                if (&(*it) == transformNode)
                {
                    transformNode->m_parent->m_children.erase(it);
                    break;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    bool        writeModel = true;
    bool        dumpModel = true;
    bool        writeTextures = true;
    bool        convertToPNG = true;
    bool        outputACC = false;
    bool        useDiffuse = true;
    bool        extractCarParts = true;
    std::string textureDirectory("textures");

    if (argc != 2)
    {
        std::cerr << "Usage: kn5 file.kn5" << std::endl;
        return EXIT_FAILURE;
    }

#if 0
    try
    {
        ini in;

        in.read("car.ini");
        in.dump();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error reading: " << "car.ini" << " : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
#endif

    kn5 model;

    const std::string file(argv[1]);

    try
    {
        model.read(file);

        if (dumpModel)
        {
            std::ofstream of(file + ".dump");

            if (of)
                model.dump(of);
        }

        if (writeModel)
        {
            kn5::Matrix xform;

            xform.m_data[0][0] = 0;
            xform.m_data[0][1] = 0;
            xform.m_data[0][2] = -1;
            xform.m_data[0][3] = 0;

            xform.m_data[1][0] = 0;
            xform.m_data[1][1] = 1;
            xform.m_data[1][2] = 0;
            xform.m_data[1][3] = 0;

            xform.m_data[2][0] = 1;
            xform.m_data[2][1] = 0;
            xform.m_data[2][2] = 0;
            xform.m_data[2][3] = 0;

            xform.m_data[3][0] = 0;
            xform.m_data[3][1] = 0;
            xform.m_data[3][2] = 0;
            xform.m_data[3][3] = 1;

            if (extractCarParts)
            {
                extract(model, "STEER_LR", xform, "steer.acc", false);
                extract(model, "STEER_HR", xform, "histeer.acc", false);

                extract(model, "WHEEL_RF", xform, "wheel0.acc", true);
                extract(model, "WHEEL_LF", xform, "wheel1.acc", true);
                extract(model, "WHEEL_RR", xform, "wheel2.acc", true);
                extract(model, "WHEEL_LR", xform, "wheel3.acc", true);
            }

            model.transform(xform);
            model.removeEmptyNodes();

            model.writeAc3d(file + (outputACC ? ".acc" : ".ac"), convertToPNG, outputACC, useDiffuse);
        }

        if (writeTextures)
        {
            std::filesystem::path directory = std::filesystem::canonical(file).parent_path();

            if (!textureDirectory.empty())
                directory.append(textureDirectory);

            model.writeTextures(directory.string(), convertToPNG);
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "Error reading: " << file << " : " << e.code().message() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error reading: " << file << " : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
