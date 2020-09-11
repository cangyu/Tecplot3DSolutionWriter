#include <iostream>
#include <cstring>
#include <filesystem>
#include "../inc/tec.h"

namespace fs = std::filesystem;

class TransientDataLoader : public DATA
{
public:
    explicit TransientDataLoader(FE_MESH_TYPE tp) : DATA(tp) {}

    void read(std::istream& in) override
    {
        int iter;
        in >> iter >> m_tm;

        size_t n_face;
        in >> m_nn >> n_face >> m_nc;

        m_var.resize(6);
        m_var[0] = new DATA_BLOCK<double>("rho", m_nn);
        m_var[1] = new DATA_BLOCK<double>("U", m_nn);
        m_var[2] = new DATA_BLOCK<double>("V", m_nn);
        m_var[3] = new DATA_BLOCK<double>("W", m_nn);
        m_var[4] = new DATA_BLOCK<double>("p", m_nn);
        m_var[5] = new DATA_BLOCK<double>("T", m_nn);

        for (size_t i = 0; i < m_nn; ++i)
            for (size_t j = 0; j < 6; ++j)
                in >> m_var[j]->data[i];
    }
};

static void convert
(
    std::ifstream &in,
    std::ofstream &out,
    FE_MESH_TYPE composition,
    const std::string &title,
    const std::string &zone_text
)
{
    DATA* loader = new TransientDataLoader(composition);
    loader->read(in);

    if (!zone_text.empty())
        loader->m_zone_text = zone_text;

    if (!title.empty())
        loader->m_title = title;

    loader->write(out);

    delete loader;
}

int main(int argc, char* argv[])
{
    std::string input_path;
    std::string output_path;
    std::string zone_text;
    std::string title;
    FE_MESH_TYPE composition;
    bool composition_flag = false;
    bool batch_mode = false;
    std::string input_dir;
    std::string output_dir;
    std::string target_suffix;

    int cnt = 1;
    while (cnt < argc)
    {
        if (!std::strcmp(argv[cnt], "--input"))
            input_path = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--output"))
            output_path = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--zone-text"))
            zone_text = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--title"))
            title = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--composition"))
        {
            std::string tmp(argv[cnt + 1]);
            if (tmp == "tet")
                composition = FE_MESH_TYPE::TET;
            else if (tmp == "hex")
                composition = FE_MESH_TYPE::HEX;
            else if (tmp == "poly")
                composition = FE_MESH_TYPE::POLY;
            else
                throw std::invalid_argument("Invalid composition description: \"" + tmp + "\".");

            composition_flag = true;
        }
        else if(!std::strcmp(argv[cnt], "--input-dir"))
        {
            input_dir = argv[cnt + 1];
            batch_mode = true;
        }
        else if(!std::strcmp(argv[cnt], "--output-dir"))
            output_dir = argv[cnt + 1];
        else if(!std::strcmp(argv[cnt], "--suffix"))
            target_suffix = argv[cnt + 1];
        else
            throw std::invalid_argument("Unrecognized option: \"" + std::string(argv[cnt]) + "\".");

        cnt += 2;
    }

    if (!composition_flag)
        throw std::runtime_error("Composition not specified!");

    if(batch_mode)
    {
        fs::path p_in(input_dir);
        if(!fs::exists(p_in))
            throw std::runtime_error("Input directory does NOT exist!");
        if(fs::directory_entry(p_in).status().type() != fs::file_type::directory)
            throw std::runtime_error("Invalid specification of input directory!");

        fs::path p_out;
        if(output_dir.empty())
            p_out = p_in;
        else
        {
            p_out = fs::path(output_dir);
            if(!fs::exists(p_out))
            {
                auto err = fs::create_directory(p_out);
                if(!err)
                    throw std::runtime_error("Failed to create output directory!");
            }
            if(fs::directory_entry(p_in).status().type() != fs::file_type::directory)
                throw std::runtime_error("Invalid specification of output directory!");
        }

        if(target_suffix.empty())
            target_suffix = ".txt";
        else
            target_suffix = "." + target_suffix;

        for(auto &it : fs::directory_iterator(p_in))
        {
            if(it.is_directory())
                continue;
            if(it.path().filename().extension().string() != target_suffix)
                continue;

            std::cout << "Converting " << it.path() << " ..." << std::endl;
            std::ifstream in(it.path());
            if(in.fail())
                throw std::runtime_error("Failed to open input file!");

            const std::string out_name = it.path().stem().string() + ".dat";
            fs::path it2 = p_out;
            it2.append(out_name);
            std::cout << "Destination: " << it2 << std::endl;
            std::ofstream out(it2);
            if(out.fail())
                throw std::runtime_error("Failed to open output file!");

            out.setf(std::ios::scientific, std::ios::floatfield);
            out.precision(15);

            convert(in, out, composition, title, it.path().stem().string());

            in.close();
            out.close();

            std::cout << "Finished!" << std::endl;
        }
    }
    else
    {
        std::ifstream in(input_path);
        if (in.fail())
            throw std::runtime_error("Failed to open input file!");

        std::ofstream out(output_path);
        if (out.fail())
            throw std::runtime_error("Failed to open output file!");

        out.setf(std::ios::scientific, std::ios::floatfield);
        out.precision(15);

        if(zone_text.empty())
            zone_text = fs::path(input_path).stem().string();

        convert(in, out, composition, title, zone_text);

        in.close();
        out.close();
    }

    return 0;
}
