#include <iomanip>
#include <cstring>
#include "../inc/tec.h"

class TransientDataLoader : public DATA
{
public:
    explicit TransientDataLoader(FE_MESH_TYPE tp) : DATA(tp) {}

    void read(std::istream &in) override
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

        for(size_t i = 0; i < m_nn; ++i)
            for(size_t j = 0; j < 6; ++j)
                in >> m_var[j]->data[i];
    }
};

int main(int argc, char *argv[])
{
    std::string input_path;
    std::string output_path;
    std::string zone_text;
    std::string title;
    FE_MESH_TYPE composition;
    DATA *loader;

    int cnt = 1;
    while (cnt < argc)
    {
        if (!std::strcmp(argv[cnt], "--input"))
            input_path = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--output"))
            output_path = argv[cnt + 1];
        else if(!std::strcmp(argv[cnt], "--zone-text"))
            zone_text = argv[cnt + 1];
        else if(!std::strcmp(argv[cnt], "--title"))
            title = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--composition"))
        {
            std::string tmp(argv[cnt + 1]);
            if(tmp == "tet")
                composition = FE_MESH_TYPE::TET;
            else if(tmp == "hex")
                composition = FE_MESH_TYPE::HEX;
            else if(tmp == "poly")
                composition = FE_MESH_TYPE::POLY;
            else
                throw std::invalid_argument("Invalid composition description: \"" + tmp + "\".");
        }
        else
            throw std::invalid_argument("Unrecognized option: \"" + std::string(argv[cnt]) + "\".");

        cnt += 2;
    }

    std::ifstream in1(input_path);
    if(in1.fail())
        return -1;

    std::ofstream out(output_path);
    if(out.fail())
        return -2;

    loader = new TransientDataLoader(composition);

    loader->read(in1);
    in1.close();

    if(!zone_text.empty())
        loader->m_zone_text = zone_text;

    if(!title.empty())
        loader->m_title = title;

    out.setf(std::ios::scientific, std::ios::floatfield);
    out.precision(15);
    loader->write(out);
    out.close();

    delete loader;
    return 0;
}
