#include "kn5.h"
#include "ini.h"

#include <fstream>
#include <filesystem>

int main(int argc, char* argv[])
{
    bool        writeModel = true;
    bool        dumpModel = true;
    bool        writeTextures = true;
    bool        convertToPNG = true;
    bool        outputACC = false;
    bool        useDiffuse = true;
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
        std::cerr << "Errro reading: " << "car.ini" << " : " << e.what() << std::endl;
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

            xform.data[0][0] = 0;
            xform.data[0][1] = 0;
            xform.data[0][2] = -1;
            xform.data[0][3] = 0;

            xform.data[1][0] = 0;
            xform.data[1][1] = 1;
            xform.data[1][2] = 0;
            xform.data[1][3] = 0;

            xform.data[2][0] = 1;
            xform.data[2][1] = 0;
            xform.data[2][2] = 0;
            xform.data[2][3] = 0;

            xform.data[3][0] = 0;
            xform.data[3][1] = 0;
            xform.data[3][2] = 0;
            xform.data[3][3] = 1;

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
        std::cerr << "Errro reading: " << file << " : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
