#include "kn5.h"
#include "ini.h"
#include "lut.h"
#include "acd.h"

#include <fstream>
#include <filesystem>

namespace
{
    void quote(std::string& string)
    {
        if (string.find(' ') != std::string::npos)
        {
            string.insert(0, 1, '"');
            string.append(1, '"');
        }
    }

    bool extract(kn5& model, const std::string& name, const kn5::Matrix& xform, const std::string& file)
    {
        kn5::Node* transformNode = model.findNode(kn5::Node::Transform, name);

        if (transformNode == nullptr)
            return false;

        kn5::Node   node = *transformNode;

        node.m_matrix.makeIdentity();

        node.transform(xform);

        model.writeAc3d(file, node, true, file.find(".acc") != std::string::npos, true);

        if (transformNode->m_parent)
            transformNode->m_parent->removeChild(transformNode);

        return true;
    }

    void remove(kn5& model, kn5::Node::NodeType type, const std::string& name)
    {
        kn5::Node* node = model.findNode(type, name);

        if (node && node->m_parent)
            node->m_parent->removeChild(node);
    }

    void writeConfig(const std::filesystem::path& inputPath, const std::string& filename, kn5& model, float length, float width, float height, const std::string& category)
    {
        std::string iniPathString = inputPath.string();

        iniPathString += std::filesystem::path::preferred_separator;

        std::filesystem::path acdPath = inputPath;

        acdPath.append("data.acd");

        if (std::filesystem::exists(acdPath))
        {
            acd data(acdPath.string());

            std::filesystem::path outputDirectory = std::filesystem::path(filename).parent_path();

            data.writeEntries(outputDirectory.string());

            iniPathString = outputDirectory.string() += std::filesystem::path::preferred_separator;
        }

        ini aero(iniPathString + "data/aero.ini");
        ini brakes(iniPathString + "data/brakes.ini");
        ini car(iniPathString + "data/car.ini");
        ini colliders(iniPathString + "data/colliders.ini");
        ini drivetrain(iniPathString + "data/drivetrain.ini");
        ini electronics;
        if (std::filesystem::exists(iniPathString + "data/electronics.ini"))
            electronics.read(iniPathString + "data/electronics.ini");
        ini engine(iniPathString + "data/engine.ini");
        ini lods(iniPathString + "data/lods.ini");
        ini flames;
        if (std::filesystem::exists(iniPathString + "data/flames.ini"))
            flames.read(iniPathString + "data/flames.ini");
        ini setup(iniPathString + "data/setup.ini");
        ini suspensions(iniPathString + "data/suspensions.ini");
        ini tires(iniPathString + "data/tyres.ini");

        std::string modelFileName = inputPath.filename().string() + ".ac";

        std::ofstream   fout(filename);

        if (!fout)
            throw std::runtime_error("Couldn't create: " + filename);

        fout << "<?xml version=\"1.0\"?>" << std::endl;
        fout << "<!DOCTYPE params SYSTEM \"../../../../src/libs/tgf/params.dtd\">" << std::endl;

        fout << "<params name=\"" << car.getValue("INFO", "SCREEN_NAME") << "\" type=\"template\">" << std::endl;

        fout << "\t<section name=\"Bonnet\">" << std::endl;
        std::array<float, 3> bonnet = car.getFloatArray3Value("GRAPHICS", "BONNET_CAMERA_POS");
        fout << "\t\t<attnum name=\"xpos\" unit=\"m\" val=\"" << bonnet[2] << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"ypos\" unit=\"m\" val=\"" << bonnet[0] << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"zpos\" unit=\"m\" val=\"" << bonnet[1] << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Driver\">" << std::endl;
        std::array<float, 3> driverEyes = car.getFloatArray3Value("GRAPHICS", "DRIVEREYES");
        fout << "\t\t<attnum name=\"xpos\" unit=\"m\" val=\"" << driverEyes[2] << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"ypos\" unit=\"m\" val=\"" << driverEyes[0] << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"zpos\" unit=\"m\" val=\"" << driverEyes[1] << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Sound\">" << std::endl;
        //	<attstr name="engine sample" val="aichiv10.wav"/>
        //	<attnum name="rpm	scale" val="0.37"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Graphic Objects\">" << std::endl;
        fout << "\t\t<attstr name=\"env\" val=\"" << modelFileName << "\"/>" << std::endl;
        //	<attstr name="wheel texture"	val="tex-wheel.rgb"/>
        //	<attstr name="shadow	texture" val="shadow.rgb"/>
        //	<attstr name="tachometer	texture" val="rpm20000.rgb"/>
        //	<attnum name="tachometer	min	value" val="0" unit="rpm"/>
        //	<attnum name="tachometer	max	value" val="20000" unit="rpm"/>
        //	<attstr name="speedometer texture" val="speed360.rgb"/>
        //	<attnum name="speedometer min value"	val="0"	unit="km/h"/>
        //	<attnum name="speedometer max value"	val="360" unit="km/h"/>
        fout << "\t\t<section name=\"Ranges\">" << std::endl;
        fout << "\t\t\t<section name=\"1\">" << std::endl;
        fout << "\t\t\t\t<attnum name=\"threshold\" val=\"0\"/>" << std::endl;
        fout << "\t\t\t\t<attstr name=\"car\" val=\"" << modelFileName << "\"/>" << std::endl;
        //    fout << "\t\t\t\t<attstr name=\"wheels\" val=\"yes\"/>" << std::endl;
        fout << "\t\t\t</section>" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name=\"Light\">" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name=\"Steer Wheel\">" << std::endl;
        fout << "\t\t\t<attstr name=\"model\" val=\"steer.acc\"/>" << std::endl;
        fout << "\t\t\t<attstr name=\"hi res model\" val=\"histeer.acc\"/>" << std::endl;
        kn5::Vec3   steer = { 0, 0, 0 };
        kn5::Node* steerNode = model.findNode(kn5::Node::Transform, "STEER_LR");

        if (steerNode)
        {
            steer[0] = steerNode->m_matrix.m_data[3][2];
            steer[1] = steerNode->m_matrix.m_data[3][0];
            steer[2] = steerNode->m_matrix.m_data[3][1];

            fout << "\t\t\t<attnum name=\"xpos\" val=\"" << steer[0] << "\"/>" << std::endl;
            fout << "\t\t\t<attnum name=\"ypos\" val=\"" << steer[1] << "\"/>" << std::endl;
            fout << "\t\t\t<attnum name=\"zpos\" val=\"" << steer[2] << "\"/>" << std::endl;
        }
        //	<attnum name="angle" val="0"/>
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name=\"Driver\">" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Car\">" << std::endl;
        fout << "\t\t<attstr name=\"category\" val=\"" << category << "\"/>" << std::endl;
        //    fout << "\t\t<attnum name=\"body length\" unit=\"m\" val=\"" << 4.86 << "\"/>" << std::endl;
        //    fout << "\t\t<attnum name=\"body width\" unit=\"m\" val=\"" << 2.00 << "\"/>" << std::endl;
        //    fout << "\t\t<attnum name=\"body height\" unit=\"m\" val=\"" << 1.05 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"overall length\" unit=\"m\" val=\"" << length << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"overall width\" unit=\"m\" val=\"" << width << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"mass\" unit=\"kg\" val=\"" << car.getValue("BASIC", "TOTALMASS") << "\"/>" << std::endl;
        //    fout << "\t\t<attnum name=\"GC height\" unit=\"m\" val=\"" << 0.24 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front-rear weight repartition\" val=\"" << suspensions.getValue("BASIC", "CG_LOCATION") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"rear right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
        //    fout << "\t\t<attnum name=\"mass repartition coefficient\" val=\"" << 0.8 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"fuel tank\" unit=\"l\" val=\"" << car.getValue("FUEL", "MAX_FUEL") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"initial fuel\" unit=\"l\" min=\"1.0\" max=\"" << car.getValue("FUEL", "MAX_FUEL") << "\" val=\"" << car.getValue("FUEL", "FUEL") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Exhaust\">" << std::endl;
        //	<attnum name="power" val="1.5"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Aerodynamics\">" << std::endl;
        //	<attnum name="Cx" val="0.347"/>
        //	<attnum name="front area" unit="m2" val="1.97"/>
        //	<attnum name="front Clift" min="0.0" max="1.0" val="0.7"/>
        //	<attnum name="rear Clift" min="0.0" max="1.0" val="0.68"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Wing\">" << std::endl;
        //	<attnum name="area" unit="m2" val="0.23"/>
        //	<attnum name="angle" unit="deg" min="0" max="12" val="7"/>
        //	<attnum name="xpos" unit="m" val="2.23"/>
        //	<attnum name="zpos" unit="m" val="0.05"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Wing\">" << std::endl;
        //	<attnum name="area" unit="m2" val="0.68"/>
        //	<attnum name="angle" unit="deg" min="0" max="18" val="13"/>
        //	<attnum name="xpos" unit="m" val="-1.95"/>
        //	<attnum name="zpos" unit="m" val="0.95"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Engine\">" << std::endl;
        //	<attnum name="capacity" unit="l" val="3.6"/>
        //	<attnum name="cylinders" val="6"/>
        //	<attstr name="shape" in="v,l,h,w" val="h"/>
        //	<attstr name="position" in="front,front-mid,mid,rear-mid,rear," val="rear"/>
        //	fout << "\t\t<attnum name=\"revs maxi\" unit=\"rpm\" min=\"5000\" max=\"10000\" val=\"" << ? << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"revs limiter\" unit=\"rpm\" val=\"" << engine.getValue("ENGINE_DATA", "LIMITER") << "\"/>" << std::endl;
        //	<attnum name="tickover" unit="rpm" min="900" max="3000" val="1500"/>
        //	<attnum name="fuel cons factor" min="1.1" max="1.3" val="1.13"/>
        //	<attstr name="turbo" val="true"/>
        //	<attnum name="turbo rpm" unit="rpm" val="3000"/>
        //	<attnum name="turbo factor" val="1.0"/>
        //	<attnum name="turbo lag" val="1.0"/>

        fout << "\t\t<section name=\"data points\">" << std::endl;
        std::filesystem::path powerPath = iniPathString;
        powerPath.append("data");
        powerPath.append(engine.getValue("HEADER", "POWER_CURVE"));
        lut power(powerPath.string());
        const std::vector<std::pair<float, float>>& values = power.getValues();
        for (size_t i = 0; i < values.size(); i++)
        {
            // TODO this is torque at wheels
            fout << "\t\t\t<section name=\"" << (i + 1) << "\">" << std::endl;
            fout << "\t\t\t\t<attnum name=\"rpm\" unit=\"rpm\" val=\"" << values[i].first << "\"/>" << std::endl;
            fout << "\t\t\t\t<attnum name=\"Tq\" unit=\"N.m\" val=\"" << values[i].second << "\"/>" << std::endl;
            fout << "\t\t\t</section>" << std::endl;
        }
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Clutch\">" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.1150"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Gearbox\">" << std::endl;
        //	<attnum name="shift time" unit="s" val="0.15"/>
        fout << "\t\t<section name=\"gears\">" << std::endl;
        fout << "\t\t\t<section name=\"r\">" << std::endl;
        fout << "\t\t\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "GEAR_R") << "\"/>" << std::endl;
        //	<attnum name="inertia" val="0.0037"/>
        //	<attnum name="efficiency" val="0.954"/>
        fout << "\t\t\t</section>" << std::endl;
        size_t gears = drivetrain.getIntValue("GEARS", "COUNT");
        for (size_t i = 0; i < gears; i++)
        {
            fout << "\t\t\t<section name=\"" << (i + 1) << "\">" << std::endl;
            fout << "\t\t\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "GEAR_" + std::to_string(i + 1)) << "\"/>" << std::endl;
            //		<attnum name="inertia" val="0.0037"/>
            //		<attnum name="efficiency" val="0.954"/>
            fout << "\t\t\t</section>" << std::endl;
        }
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Drivetrain\">" << std::endl;
        std::string traction = drivetrain.getValue("TRACTION", "TYPE");
        if (traction == "AWD")
            traction = "4WD";
        fout << "\t\t<attstr name=\"type\" val=\"" << traction << "\"/>" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.0091"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Steer\">" << std::endl;
        //	<attnum name="steer lock" unit="deg" min="1" max="21" val="21"/>
        //	<attnum name="steering wheel rotation" unit="deg" min="5.456741" max="114.591568" val="114.591560"/>
        //	<attnum name="max steer speed" unit="deg/s" min="1" max="360" val="360"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Brake System\">" << std::endl;
        if (brakes.getIntValue("DATA", "COCKPIT_ADJUSTABLE") == 1)
            fout << "\t\t<attnum name=\"front-rear brake repartition\" min=\"0.3\" max=\"0.7\" val=\"" << brakes.getValue("DATA", "FRONT_SHARE") << "\"/>" << std::endl;
        else
            fout << "\t\t<attnum name=\"front-rear brake repartition\" val=\"" << brakes.getValue("DATA", "FRONT_SHARE") << "\"/>" << std::endl;
        //	<attnum name="max pressure" unit="kPa" min="100" max="150000" val="25000"/>
        fout << "\t</section>" << std::endl;

        float wheelbase = suspensions.getFloatValue("BASIC", "WHEELBASE");
        float cg = suspensions.getFloatValue("BASIC", "CG_LOCATION");

        fout << "\t<section name=\"Front Axle\">" << std::endl;
        fout << "\t\t<attnum name=\"xpos\" val=\"" << (wheelbase * cg) << "\"/>" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.0056"/>
        //	<attnum name="roll center height" unit="m" min="0" max="0.5" val="0.11"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Axle\">" << std::endl;
        fout << "\t\t<attnum name=\"xpos\" val=\"" << ((cg - 1) * wheelbase) << "\"/>" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.0080"/>
        //	<attnum name="roll center height" unit="m" min="0" max="0.5" val="0.14"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Differential\">" << std::endl;
        //	<attstr name="type" in="SPOOL,FREE,LIMITED SLIP" val="LIMITED SLIP"/>
        if (drivetrain.getValue("TRACTION", "TYPE") != "RWD")
            fout << "\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "FINAL") << "\"/>" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.0488"/>
        //	<attnum name="efficiency" val="1.0"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Differential\">" << std::endl;
        //	<attstr name="type" in="SPOOL,FREE,LIMITED SLIP" val="LIMITED SLIP"/>
        if (drivetrain.getValue("TRACTION", "TYPE") != "FWD")
            fout << "\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "FINAL") << "\"/>" << std::endl;
        //	<attnum name="inertia" unit="kg.m2" val="0.0488"/>
        //	<attnum name="efficiency" val="1.0"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Central Differential\">" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Wheel\">" << std::endl;
        //  fout << "\t\t<attnum name = "ypos" unit = "m" val = "-0.83" / >" << std::endl;
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("FRONT", "WIDTH") << "\"/>" << std::endl;
        //  fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RADIUS") * 2.0f) << "\"/>" << std::endl;
        float tireAspectRatio = (tires.getFloatValue("FRONT", "RADIUS") - tires.getFloatValue("FRONT", "RIM_RADIUS")) / tires.getFloatValue("FRONT", "WIDTH");
        fout << "\t\t<attnum name=\"tire height-width ratio\" val=\"" << tireAspectRatio << "\"/>" << std::endl;
        //  fout << "\t\t<attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >" << std::endl;
        //  fout << "\t\t<attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >" << std::endl;
        //  fout << "\t\t<attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >" << std::endl;
        //  fout << "\t\t<attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >" << std::endl;
        //  fout << "\t\t<attnum name = "stiffness" val = "27.0" / >" << std::endl;
        //  fout << "\t\t<attnum name = "dynamic friction" unit = "%" val = "80" / >" << std::endl;
        //  fout << "\t\t<attnum name = "rolling resistance" val = "0.02" / >" << std::endl;
        //  fout << "\t\t<attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Wheel\">" << std::endl;
        //  <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("FRONT", "WIDTH") << "\"/>" << std::endl;
        //  fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height-width ratio\" val=\"" << tireAspectRatio << "\"/>" << std::endl;
        //  <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
        //  <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
        //  <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
        //  <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
        //  <attnum name = "stiffness" val = "27.0" / >
        //  <attnum name = "dynamic friction" unit = "%" val = "80" / >
        //  <attnum name = "rolling resistance" val = "0.02" / >
        //  <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Wheel\">" << std::endl;
        //  <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("REAR", "WIDTH") << "\"/>" << std::endl;
        //  fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RADIUS") * 2.0f) << "\"/>" << std::endl;
        tireAspectRatio = (tires.getFloatValue("REAR", "RADIUS") - tires.getFloatValue("REAR", "RIM_RADIUS")) / tires.getFloatValue("REAR", "WIDTH");
        fout << "\t\t<attnum name=\"tire height-width ratio\" val=\"" << tireAspectRatio << "\"/>" << std::endl;
        //  <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
        //  <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
        //  <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
        //  <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
        //  <attnum name = "stiffness" val = "27.0" / >
        //  <attnum name = "dynamic friction" unit = "%" val = "80" / >
        //  <attnum name = "rolling resistance" val = "0.02" / >
        //  <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Wheel\">" << std::endl;
        //  <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("REAR", "WIDTH") << "\"/>" << std::endl;
        //  fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height-width ratio\" val=\"" << tireAspectRatio << "\"/>" << std::endl;
        //  <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
        //  <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
        //  <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
        //  <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
        //  <attnum name = "stiffness" val = "27.0" / >
        //  <attnum name = "dynamic friction" unit = "%" val = "80" / >
        //  <attnum name = "rolling resistance" val = "0.02" / >
        //  <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Anti-Roll Bar\">" << std::endl;
        fout << "\t\t<attnum name=\"spring\" unit=\"N/m\" min=\"0\" max=\"50000\" val=\"" << suspensions.getValue("ARB", "FRONT") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Anti-Roll Bar\">" << std::endl;
        fout << "\t\t<attnum name=\"spring\" unit=\"N/m\" min=\"0\" max=\"50000\" val=\"" << suspensions.getValue("ARB", "REAR") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Suspension\">" << std::endl;
        //	<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
        //	<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
        //	<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
        //	<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
        //	<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
        //	<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
        //	<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
        //	<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Suspension\">" << std::endl;
        //	<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
        //	<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
        //	<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
        //	<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
        //	<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
        //	<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
        //	<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
        //	<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Suspension\">" << std::endl;
        //	<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
        //	<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
        //	<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
        //	<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
        //	<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
        //	<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
        //	<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
        //	<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Suspension\">" << std::endl;
        //	<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
        //	<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
        //	<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
        //	<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
        //	<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
        //	<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
        //	<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
        //	<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Brake\">" << std::endl;
        std::string nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_RF");
        const kn5::Node* disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
        //	<attnum name="piston area" unit="cm2" val="50"/>
        //	<attnum name="mu" val="0.45"/>
        //	<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_LF");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
        //	<attnum name="piston area" unit="cm2" val="50"/>
        //	<attnum name="mu" val="0.45"/>
        //	<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_RR");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
        //	<attnum name="piston area" unit="cm2" val="50"/>
        //	<attnum name="mu" val="0.45"/>
        //	<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_LR");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
        //	<attnum name="piston area" unit="cm2" val="50"/>
        //	<attnum name="mu" val="0.45"/>
        //	<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "</params>" << std::endl;

        fout.close();
    }

    void usage()
    {
        std::cout << "Usage: kn5toac -c category -i input_directory [-o output_directory] [-n kn5_filename] [-s skin_filename] [-d]" << std::endl;
        std::cout << " -c category           Speed Dreams category to add this car to." << std::endl;
        std::cout << " -i input_directory    Assetto Corsa car directory of car to convert." << std::endl;
        std::cout << " -o output_directory   Speed Dreams directory where converted car will be installed." << std::endl;
        std::cout << " -n kn5_filename       Assetto Corsa car model to convert if different form directory name." << std::endl;
        std::cout << " -s skin_filename      Assetto Corsa skin texture file name" << std::endl;
        std::cout << " -d                    Dumps kn5 files and doesn't delete dds textures and data directory." << std::endl;
    }
}

int main(int argc, char* argv[])
{
    bool        writeModel = false;
    bool        dumpModel = false;
    bool        writeTextures = false;
    bool        convertToPNG = false;
    bool        deleteDDS = true;
    bool        outputACC = false;
    bool        useDiffuse = false;
    bool        extractCarParts = false;
    bool        writeCarConfig = false;
    bool        dumpCollider = false;
    bool        writeCmake = false;
    bool        deleteData = true;
    bool        writeSkins = false;
    std::string category;
    std::string inputDirectory;
    std::string outputDirectory;
    std::string inputFileName;
    std::string skinFileName;

    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg == "-c")
        {
            if (i < argc)
            {
                i++;
                category = argv[i];
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-d")
        {
            dumpModel = true;
            dumpCollider = true;
            deleteData = false;
            deleteDDS = false;
        }
        else if (arg == "-h")
        {
            usage();
            return EXIT_SUCCESS;
        }
        else if (arg == "-i")
        {
            if (i < argc)
            {
                i++;
                inputDirectory = argv[i];
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-n")
        {
            if (i < argc)
            {
                i++;
                inputFileName = argv[i];
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-o")
        {
            if (i < argc)
            {
                i++;
                outputDirectory = argv[i];
                writeTextures = true;
                convertToPNG = true;
                writeModel = true;
                extractCarParts = true;
                writeCarConfig = true;
                useDiffuse = true;
                writeCmake = true;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (arg == "-s")
        {
            if (i < argc)
            {
                i++;
                skinFileName = argv[i];
                writeSkins = true;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else
        {
            std::cerr << "Unknown option: " << arg << std::endl;
            usage();
            return EXIT_FAILURE;
        }
    }

    std::filesystem::path   inputPath(inputDirectory);
    std::filesystem::path   outputPath(outputDirectory);

    const std::string       inputFileDirectoryName(inputPath.filename().string());

    outputPath.append(inputFileDirectoryName);

    if (inputFileName.empty())
        inputFileName = inputFileDirectoryName + ".kn5";

    std::filesystem::path   inputFilePath(inputPath);

    inputFilePath.append(inputFileName);

    kn5 model;

    try
    {
        model.read(inputFilePath.string());
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "Error reading: " << inputFilePath.string() << " : " << e.code().message() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error reading: " << inputFilePath.string() << " : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(outputPath))
    {
        if (!std::filesystem::create_directory(outputPath))
        {
            std::cerr << "Couldn't create: " << outputPath.string() << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (dumpModel)
    {
        std::filesystem::path dumpFilePath = outputPath;

        dumpFilePath.append(inputFileName + ".dump");

        std::ofstream of(dumpFilePath.string());

        if (of)
            model.dump(of);
    }

    if (writeCarConfig)
    {
        std::filesystem::path   colliderFilePath = inputPath;

        colliderFilePath.append("collider.kn5");

        kn5 collider;

        try
        {
            collider.read(colliderFilePath.string());
        }
        catch (std::ifstream::failure& e)
        {
            std::cerr << "Error reading: " << colliderFilePath.string() << " : " << e.code().message() << std::endl;
            return EXIT_FAILURE;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << "Error reading: " << colliderFilePath.string() << " : " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        if (dumpCollider)
        {
            std::filesystem::path dumpFilePath = outputPath;

            dumpFilePath.append("collider.kn5.dump");

            std::ofstream of1(dumpFilePath.string());

            if (of1)
                collider.dump(of1);
        }

        kn5::Vec3 minimum = { 100000, 100000, 100000 };
        kn5::Vec3 maximum = { -100000, -100000, -100000 };

        kn5::Node& mesh = collider.m_node;
        while (mesh.m_type == kn5::Node::Transform && mesh.m_children.size() == 1)
            mesh = mesh.m_children[0];

        if (mesh.m_type == kn5::Node::Mesh)
        {
            for (const auto& vertex : mesh.m_vertices)
            {
                for (size_t i = 0; i < 3; i++)
                {
                    if (vertex.m_position[i] < minimum[i])
                        minimum[i] = vertex.m_position[i];

                    if (vertex.m_position[i] > maximum[i])
                        maximum[i] = vertex.m_position[i];
                }
            }
        }

        float length = maximum[2] - minimum[2];
        float width = maximum[0] - minimum[0];
        float height = maximum[1] - minimum[1];

        std::filesystem::path   configFilePath = outputPath;

        configFilePath.append(inputFileDirectoryName + ".xml");

        try
        {
            writeConfig(inputPath, configFilePath.string(), model, length, width, height, category);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << "Error writing : " << configFilePath.string() << " : " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
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
            std::filesystem::path extractFilePath = outputPath;

            extractFilePath.append("steer.acc");

            extract(model, "STEER_LR", xform, extractFilePath.string());

            extractFilePath = outputPath;

            extractFilePath.append("histeer.acc");

            extract(model, "STEER_HR", xform, extractFilePath.string());

            remove(model, kn5::Node::Transform, "WHEEL_RF");
            remove(model, kn5::Node::Transform, "WHEEL_LF");
            remove(model, kn5::Node::Transform, "WHEEL_RR");
            remove(model, kn5::Node::Transform, "WHEEL_LR");

            std::filesystem::path iniFilePath = inputPath;

            iniFilePath.append("data/brakes.ini");

            if (!std::filesystem::exists(iniFilePath))
            {
                iniFilePath = outputPath;

                iniFilePath.append("data/brakes.ini");
            }

            const ini brakes(iniFilePath.string());

            remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_LF"));
            remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_RF"));
            remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_LR"));
            remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_RR"));
        }

        model.transform(xform);
        model.removeEmptyNodes();

        std::filesystem::path outputFilePath = outputPath;

        outputFilePath.append(inputFileDirectoryName + (outputACC ? ".acc" : ".ac"));

        model.writeAc3d(outputFilePath.string(), convertToPNG, outputACC, useDiffuse);
    }

    if (writeTextures)
        model.writeTextures(outputPath.string(), convertToPNG, deleteDDS);

    if (writeCmake)
    {
        std::filesystem::path cmakeFileName = outputPath;

        cmakeFileName.append("CMakeLists.txt");

        std::ofstream   fout(cmakeFileName.string());

        if (fout)
        {
            fout << "INCLUDE(../../../../cmake/macros.cmake)" << std::endl;
            fout << std::endl;
            fout << "SD_INSTALL_CAR(" << inputFileDirectoryName << ")" << std::endl;

            fout.close();
        }
    }

    if (deleteData)
    {
        std::filesystem::path dataDirectory = outputPath;

        dataDirectory.append("data");

        if (std::filesystem::exists(dataDirectory))
            std::filesystem::remove_all(dataDirectory);
    }

    if (writeSkins)
    {
        std::filesystem::path skinDirectory = inputPath;

        skinDirectory.append("skins");

        if (std::filesystem::exists(skinDirectory))
        {
            for (const auto& entry : std::filesystem::directory_iterator(skinDirectory))
            {
                std::filesystem::path   skinFilePath = entry.path();
                const std::string livery = skinFilePath.filename().string();

                skinFilePath.append(skinFileName);

                if (std::filesystem::exists(skinFilePath))
                {
                    std::filesystem::path liveryFilePath = outputPath;

                    liveryFilePath.append(inputFileDirectoryName + "-" + livery + ".png");

                    std::string liveryFilePathString = liveryFilePath.string();

                    quote(liveryFilePathString);

                    std::string skinFilePathString = skinFilePath.string();

                    quote(skinFilePathString);

                    const std::string command("magick convert " + skinFilePathString + " " + liveryFilePathString);

                    if (system(command.c_str()) == -1)
                    {
                        std::cerr << "failed to convert " << skinFilePathString << " to " << liveryFilePathString << std::endl;

                        continue;
                    }

                    std::filesystem::path previewPath = entry.path();

                    previewPath.append("preview.jpg");

                    if (std::filesystem::exists(previewPath))
                    {
                        std::filesystem::path liveryPreviewPath = outputPath;

                        liveryPreviewPath.append(inputFileDirectoryName + "-" + livery + "-preview.jpg");

                        std::filesystem::copy(previewPath, liveryPreviewPath, std::filesystem::copy_options::overwrite_existing);
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
