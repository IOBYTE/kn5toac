#include "kn5.h"
#include "ini.h"

#include <fstream>
#include <filesystem>

static void extract(kn5& model, const std::string& name, const kn5::Matrix& xform, const std::string & file)
{
    kn5::Node* transformNode = model.findNode(kn5::Node::Transform, name);

    if (transformNode)
    {
        kn5::Node   node = transformNode->m_children[0];

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

static void remove(kn5& model, kn5::Node::NodeType type, const std::string& name)
{
    kn5::Node* node = model.findNode(type, name);

    if (node)
    {
        if (node->m_parent)
        {
            for (std::vector<kn5::Node>::iterator it = node->m_parent->m_children.begin(); it != node->m_parent->m_children.end(); ++it)
            {
                if (&(*it) == node)
                {
                    node->m_parent->m_children.erase(it);
                    break;
                }
            }
        }
    }
}

static void writeConfig(const std::string& filename, kn5& model)
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
        ini lods("data/lods.ini");
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

        fout << "<params name=\"" << car.getValue("INFO", "SCREEN_NAME") << "\" type=\"template\">" << std::endl;

        fout << "\t<section name=\"Bonnet\">" << std::endl;
//		<attnum name="xpos" val="-0.25" unit="m"/>
//		<attnum name="ypos" val="0" unit="m"/>
//		<attnum name="zpos" val="0.95" unit="m"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Driver\">" << std::endl;
//      <attnum name = "xpos" val = "0.0" unit = "m" / >
//      <attnum name = "ypos" val = "0" unit = "m" / >
//      <attnum name = "zpos" val = "0.64" unit = "m" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Sound\">" << std::endl;
//	    <attstr name="engine sample" val="aichiv10.wav"/>
//	    <attnum name="rpm	scale" val="0.37"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Graphic Objects\">" << std::endl;
//	    <attstr name="env" val="formula_k.kn5.ac"/>
//	    <attstr name="wheel texture"	val="tex-wheel.rgb"/>
//	    <attstr name="shadow	texture" val="shadow.rgb"/>
//	    <attstr name="tachometer	texture" val="rpm20000.rgb"/>
//	    <attnum name="tachometer	min	value" val="0" unit="rpm"/>
//	    <attnum name="tachometer	max	value" val="20000" unit="rpm"/>
//	    <attstr name="speedometer texture" val="speed360.rgb"/>
//	    <attnum name="speedometer min value"	val="0"	unit="km/h"/>
//	    <attnum name="speedometer max value"	val="360" unit="km/h"/>
        fout << "\t\t<section name=\"Ranges\">" << std::endl;
        fout << "\t\t\t<section name=\"1\">" << std::endl;
        fout << "\t\t\t\t<attnum name=\"threshold\" val=\"0\"/>" << std::endl;
        fout << "\t\t\t\t<attstr name=\"car\" val=\"" << modelFileName << "\"/>" << std::endl;
//        fout << "\t\t\t\t<attstr name=\"wheels\" val=\"yes\"/>" << std::endl;
        fout << "\t\t\t</section>" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name = \"Light\">" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name=\"Steer Wheel\">" << std::endl;
        fout << "\t\t\t<attstr name=\"model\" val=\"steer.acc\"/>" << std::endl;
        fout << "\t\t\t<attstr name=\"hi res model\" val=\"histeer.acc\"/>" << std::endl;
//	 	<attnum name="xpos" val="0.4"/>
//	 	<attnum name="ypos" val="0"/>
//	 	<attnum name="zpos" val="0.46"/>
//	 	<attnum name="angle" val="0"/>
        fout << "\t\t</section>" << std::endl;
        fout << "\t\t<section name=\"Driver\">" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Car\">" << std::endl;
        fout << "\t\t<attstr name=\"category\" val=\"" << "Formula-K" << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"body length\" unit=\"m\" val=\"" << 4.86 << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"body width\" unit=\"m\" val=\"" << 2.00 << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"body height\" unit=\"m\" val=\"" << 1.05 << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"overall length\" unit=\"m\" val=\"" << 4.86 << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"overall width\" unit=\"m\" val=\"" << 2.00 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"mass\" unit=\"kg\" val=\"" << car.getValue("BASIC", "TOTALMASS") << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"GC height\" unit=\"m\" val=\"" << 0.24 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front-rear weight repartition\" val=\"" << suspensions.getValue("BASIC", "CG_LOCATION") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"front right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"rear right-left weight repartition\" val=\"" << 0.5 << "\"/>" << std::endl;
//        fout << "\t\t<attnum name=\"mass repartition coefficient\" val=\"" << 0.8 << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"fuel tank\" unit=\"l\" val=\"" << car.getValue("FUEL", "MAX_FUEL") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"initial fuel\" unit=\"l\" min=\"1.0\" max=\"" << car.getValue("FUEL", "MAX_FUEL") << "\" val=\"" << car.getValue("FUEL", "FUEL") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Exhaust\">" << std::endl;
//		<attnum name="power" val="1.5"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Aerodynamics\">" << std::endl;
//		<attnum name="Cx" val="0.347"/>
//		<attnum name="front area" unit="m2" val="1.97"/>
//		<attnum name="front Clift" min="0.0" max="1.0" val="0.7"/>
//		<attnum name="rear Clift" min="0.0" max="1.0" val="0.68"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Wing\">" << std::endl;
//		<attnum name="area" unit="m2" val="0.23"/>
//		<attnum name="angle" unit="deg" min="0" max="12" val="7"/>
//		<attnum name="xpos" unit="m" val="2.23"/>
//		<attnum name="zpos" unit="m" val="0.05"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Wing\">" << std::endl;
//		<attnum name="area" unit="m2" val="0.68"/>
//		<attnum name="angle" unit="deg" min="0" max="18" val="13"/>
//		<attnum name="xpos" unit="m" val="-1.95"/>
//		<attnum name="zpos" unit="m" val="0.95"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Engine\">" << std::endl;
//		<attnum name="capacity" unit="l" val="3.6"/>
//		<attnum name="cylinders" val="6"/>
//		<attstr name="shape" in="v,l,h,w" val="h"/>
//		<attstr name="position" in="front,front-mid,mid,rear-mid,rear," val="rear"/>
//		<attnum name="revs maxi" unit="rpm" min="5000" max="10000" val="9500"/>
//		<attnum name="revs limiter" unit="rpm" min="5000" max="9000" val="8500"/>
//		<attnum name="tickover" unit="rpm" min="900" max="3000" val="1500"/>
//		<attnum name="fuel cons factor" min="1.1" max="1.3" val="1.13"/>
//		<attstr name="turbo" val="true"/>
//		<attnum name="turbo rpm" unit="rpm" val="3000"/>
//		<attnum name="turbo factor" val="1.0"/>
//		<attnum name="turbo lag" val="1.0"/>
        fout << "\t\t<section name = \"data points\">" << std::endl;
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Clutch\">" << std::endl;
//		<attnum name="inertia" unit="kg.m2" val="0.1150"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Gearbox\">" << std::endl;
//		<attnum name="shift time" unit="s" val="0.15"/>
        fout << "\t\t<section name = \"gears\">" << std::endl;
        fout << "\t\t\t<section name=\"r\">" << std::endl;
        fout << "\t\t\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "GEAR_R") << "\"/>" << std::endl;
//		<attnum name="inertia" val="0.0037"/>
//		<attnum name="efficiency" val="0.954"/>
        fout << "\t\t\t</section>" << std::endl;
        size_t gears = drivetrain.getIntValue("GEARS", "COUNT");
        for (size_t i = 0; i < gears; i++)
        {
            fout << "\t\t\t<section name=\"" << (i + 1) << "\">" << std::endl;
            fout << "\t\t\t\t<attnum name=\"ratio\" val=\"" << drivetrain.getValue("GEARS", "GEAR_" + std::to_string(i + 1)) << "\"/>" << std::endl;
//			<attnum name="inertia" val="0.0037"/>
//			<attnum name="efficiency" val="0.954"/>
            fout << "\t\t\t</section>" << std::endl;
        }
        fout << "\t\t</section>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Drivetrain\">" << std::endl;
        std::string traction = drivetrain.getValue("TRACTION", "TYPE");
        if (traction == "AWD")
            traction = "4WD";
		fout << "\t\t<attstr name=\"type\" val=\"" << traction << "\"/>" << std::endl;
//		<attnum name="inertia" unit="kg.m2" val="0.0091"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Steer\">" << std::endl;
//		<attnum name="steer lock" unit="deg" min="1" max="21" val="21"/>
//		<attnum name="steering wheel rotation" unit="deg" min="5.456741" max="114.591568" val="114.591560"/>
//		<attnum name="max steer speed" unit="deg/s" min="1" max="360" val="360"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Brake System\">" << std::endl;
        if (brakes.getIntValue("DATA", "COCKPIT_ADJUSTABLE") == 1)
		    fout << "\t\t<attnum name=\"front-rear brake repartition\" min=\"0.3\" max=\"0.7\" val=\"" << brakes.getValue("DATA", "FRONT_SHARE") << "\"/>" << std::endl;
        else
            fout << "\t\t<attnum name=\"front-rear brake repartition\" val=\"" << brakes.getValue("DATA", "FRONT_SHARE") << "\"/>" << std::endl;
//		<attnum name="max pressure" unit="kPa" min="100" max="150000" val="25000"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Axle\">" << std::endl;
//		fout << "\t\t<attnum name=\"xpos\" min=\"0.5\" max=\"2.5\" val=\"" << 1.22 "\"/>" << std::endl;
//		<attnum name="inertia" unit="kg.m2" val="0.0056"/>
//		<attnum name="roll center height" unit="m" min="0" max="0.5" val="0.11"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Axle\">" << std::endl;
//		fout << "\t\t<attnum name=\"xpos\" min=\"-2.5\" max=\"-0.5\" val=\"" << -1.14 << "\"/>" << std::endl;
//		<attnum name="inertia" unit="kg.m2" val="0.0080"/>
//		<attnum name="roll center height" unit="m" min="0" max="0.5" val="0.14"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Differential\">" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Differential\">" << std::endl;
//		<attstr name="type" in="SPOOL,FREE,LIMITED SLIP" val="LIMITED SLIP"/>
//		<attnum name="inertia" unit="kg.m2" val="0.0488"/>
//		<attnum name="ratio" min="0" max="10" val="3.07"/>
//		<attnum name="efficiency" val="1.0"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Central Differential\">" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Wheel\">" << std::endl;
//      fout << "\t\t<attnum name = "ypos" unit = "m" val = "-0.83" / >" << std::endl;
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("FRONT", "WIDTH") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RADIUS") * 2.0f) << "\"/>" << std::endl;
//      fout << "\t\t<attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >" << std::endl;
//      fout << "\t\t<attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >" << std::endl;
//      fout << "\t\t<attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >" << std::endl;
//      fout << "\t\t<attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >" << std::endl;
//      fout << "\t\t<attnum name = "stiffness" val = "27.0" / >" << std::endl;
//      fout << "\t\t<attnum name = "dynamic friction" unit = "%" val = "80" / >" << std::endl;
//      fout << "\t\t<attnum name = "rolling resistance" val = "0.02" / >" << std::endl;
//      fout << "\t\t<attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Wheel\">" << std::endl;
//      <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("FRONT", "WIDTH") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("FRONT", "RADIUS") * 2.0f) << "\"/>" << std::endl;
//      <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
//      <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
//      <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
//      <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
//      <attnum name = "stiffness" val = "27.0" / >
//      <attnum name = "dynamic friction" unit = "%" val = "80" / >
//      <attnum name = "rolling resistance" val = "0.02" / >
//      <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Wheel\">" << std::endl;
//      <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("REAR", "WIDTH") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RADIUS") * 2.0f) << "\"/>" << std::endl;
//      <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
//      <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
//      <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
//      <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
//      <attnum name = "stiffness" val = "27.0" / >
//      <attnum name = "dynamic friction" unit = "%" val = "80" / >
//      <attnum name = "rolling resistance" val = "0.02" / >
//      <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Wheel\">" << std::endl;
//      <attnum name = "ypos" unit = "m" val = "-0.83" / >
        fout << "\t\t<attnum name=\"rim diameter\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RIM_RADIUS") * 2.0f) << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire width\" unit=\"m\" val=\"" << tires.getValue("REAR", "WIDTH") << "\"/>" << std::endl;
        fout << "\t\t<attnum name=\"tire height\" unit=\"m\" val=\"" << (tires.getFloatValue("REAR", "RADIUS") * 2.0f) << "\"/>" << std::endl;
//      <attnum name = "inertia" unit = "kg.m2" val = "1.2200" / >
//      <attnum name = "ride height" unit = "mm" min = "100" max = "300" val = "100" / >
//      <attnum name = "toe" unit = "deg" min = "-5" max = "5" val = "0" / >
//      <attnum name = "camber" min = "-5" max = "0" unit = "deg" val = "-5" / >
//      <attnum name = "stiffness" val = "27.0" / >
//      <attnum name = "dynamic friction" unit = "%" val = "80" / >
//      <attnum name = "rolling resistance" val = "0.02" / >
//      <attnum name = "mu" min = "0.05" max = "1.6" val = "1.6" / >
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Anti-Roll Bar\">" << std::endl;
        fout << "\t\t<attnum name=\"spring\" unit=\"N/m\" min=\"0\" max=\"50000\" val=\"" << suspensions.getValue("ARB", "FRONT") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Anti-Roll Bar\">" << std::endl;
        fout << "\t\t<attnum name=\"spring\" unit=\"N/m\" min=\"0\" max=\"50000\" val=\"" << suspensions.getValue("ARB", "REAR") << "\"/>" << std::endl;
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Suspension\">" << std::endl;
//		<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
//		<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
//		<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
//		<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
//		<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
//		<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
//		<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
//		<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Suspension\">" << std::endl;
//		<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
//		<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
//		<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
//		<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
//		<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
//		<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
//		<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
//		<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Suspension\">" << std::endl;
//		<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
//		<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
//		<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
//		<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
//		<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
//		<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
//		<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
//		<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Suspension\">" << std::endl;
//		<attnum name="spring" unit="lbs/in" min="0" max="10000" val="5500"/>
//		<attnum name="suspension course" unit="m" min="0" max="0.25" val="0.07"/>
//		<attnum name="bellcrank" min="0.1" max="5" val="0.9"/>
//		<attnum name="packers" unit="mm" min="0" max="20" val="10"/>
//		<attnum name="slow bump" unit="lbs/in/s" min="0" max="1000" val="500"/>
//		<attnum name="slow rebound" unit="lbs/in/s" min="0" max="1000" val="300"/>
//		<attnum name="fast bump" unit="lbs/in/s" min="0" max="1000" val="60"/>
//		<attnum name="fast rebound" unit="lbs/in/s" min="0" max="1000" val="60"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Right Brake\">" << std::endl;
        std::string nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_RF");
        const kn5::Node* disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
//		<attnum name="piston area" unit="cm2" val="50"/>
//		<attnum name="mu" val="0.45"/>
//		<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Front Left Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_LF");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
//		<attnum name="piston area" unit="cm2" val="50"/>
//		<attnum name="mu" val="0.45"/>
//		<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Right Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_RR");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
 //		<attnum name="piston area" unit="cm2" val="50"/>
//		<attnum name="mu" val="0.45"/>
//		<attnum name="inertia" unit="kg.m2" val="0.1241"/>
        fout << "\t</section>" << std::endl;

        fout << "\t<section name=\"Rear Left Brake\">" << std::endl;
        nodeName = brakes.getValue("DISCS_GRAPHICS", "DISC_LR");
        disk = model.findNode(kn5::Node::Transform, nodeName);
        if (disk && disk->m_children.size() == 1 && disk->m_children[0].m_type == kn5::Node::Mesh)
            fout << "\t\t<attnum name=\"disk diameter\" unit=\"m\" val=\"" << (disk->m_children[0].m_boundingSphere.m_radius * 2.0f) << "\"/>" << std::endl;
 //		<attnum name="piston area" unit="cm2" val="50"/>
//		<attnum name="mu" val="0.45"/>
//		<attnum name="inertia" unit="kg.m2" val="0.1241"/>
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

        if (writeCarConfig)
        {
            writeConfig("formula_k.xml", model);
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
                extract(model, "STEER_LR", xform, "steer.acc");
                extract(model, "STEER_HR", xform, "histeer.acc");

                remove(model, kn5::Node::Transform, "WHEEL_RF");
                remove(model, kn5::Node::Transform, "WHEEL_LF");
                remove(model, kn5::Node::Transform, "WHEEL_RR");
                remove(model, kn5::Node::Transform, "WHEEL_LR");

                ini brakes("data/brakes.ini");

                remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_LF"));
                remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_RF"));
                remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_LR"));
                remove(model, kn5::Node::Mesh, brakes.getValue("DISCS_GRAPHICS", "DISC_RR"));
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
