#include "kn5.h"
#include "ini.h"

#include <fstream>
#include <filesystem>

static void extract(kn5& model, const std::string& name, const kn5::Matrix& xform, const std::string & file, bool wheels)
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

static void writeConfig(const std::string& filename)
{
    try
    {
        ini aero("data/aero.ini");
        ini brakes("data/brakes.ini");
        ini car("data/car.ini");
        ini colliders("data/colliders.ini");
        ini drivetrain("data/drivetrain.ini");
        ini electronics("data/electronics.ini");
        ini engine("data/engine.ini");
        ini flames("data/flames.ini");
        ini setup("data/setup.ini");
        ini suspensions("data/suspensions.ini");
        ini tires("data/tyres.ini");

        std::string modelFileName = "formula_k.kn5.ac";

        std::ofstream   fout(filename);

        if (!fout)
            throw std::runtime_error("Couldn't create: " + filename);

        fout << "<?xml version=\"1.0\"?>" << std::endl;
        fout << "<!DOCTYPE params SYSTEM \"../../../../src/libs/tgf/params.dtd\">" << std::endl;

        fout << "<params name = \"" << car.getValue("INFO", "SCREEN_NAME") << "\" type = \"template\">" << std::endl;

        fout << "\t<section name=\"Graphic Objects\">" << std::endl;
        fout << "\t\t<section name=\"Ranges\">" << std::endl;
        fout << "\t\t\t<section name=\"1\">" << std::endl;
        fout << "\t\t\t\t<attnum name=\"threshold\" val=\"0\"/>" << std::endl;
        fout << "\t\t\t\t<attstr name=\"car\" val=\"" << modelFileName << "\"/>" << std::endl;
        fout << "\t\t\t\t<attstr name=\"wheels\" val=\"yes\"/>" << std::endl;
        fout << "\t\t\t</section>" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Car\">" << std::endl;
        fout << "\t\t<attstr name=\"category\" val=\"" << "Supercars" << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"body length\" unit=\"m\" val=\"" << 4.86 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"body width\" unit=\"m\" val=\"" << 2.00 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"body height\" unit=\"m\" val=\"" << 1.05 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"overall length\" unit=\"m\" val=\"" << 4.86 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"overall width\" unit=\"m\" val=\"" << 2.00 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"mass\" unit=\"kg\" val=\"" << car.getValue("BASIC", "TOTALMASS") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"GC height\" unit=\"m\" val=\"" << 0.24 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front-rear weight repartition\" val=\"" << 0.48 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"rear right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"mass repartition coefficient\" val=\"" << 0.8 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"fuel tank\" unit=\"l\" val=\"" << car.getValue("FUEL", "FUEL") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"initial fuel\" unit=\"l\" min=\"1.0\" max=\"" << car.getValue("FUEL", "MAX_FUEL") << "\" val=\"" << car.getValue("FUEL", "FUEL") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "</params>" << std::endl;

        fout.close();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error : " << e.what() << std::endl;
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
    bool        writeCarConfig = true;
    std::string textureDirectory("textures");

    if (argc != 2)
    {
        std::cerr << "Usage: kn5 file.kn5" << std::endl;
        return EXIT_FAILURE;
    }

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

        if (writeCarConfig)
        {
            writeConfig("formula_k.xml");
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
