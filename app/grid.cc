#include <iostream>
#include <cstring>
#include "../inc/tec.h"

class HomogeneousMeshLoader : public GRID
{
private:
    size_t NODE_PER_CELL;
    std::vector<size_t> m_connect;

public:
    explicit HomogeneousMeshLoader(FE_MESH_TYPE tp) :GRID(tp)
    {
        switch (m_tp)
        {
        case FE_MESH_TYPE::TET:
            NODE_PER_CELL = 4;
            break;
        case FE_MESH_TYPE::HEX:
            NODE_PER_CELL = 8;
            break;
        case FE_MESH_TYPE::POLY:
            throw std::invalid_argument("Wrong mesh type!");
        default:
            break;
        }
    }

    void write_connectivity(std::ostream& out) override
    {
        static const char SEP = ' ';

        for (size_t i = 0; i < m_nc; ++i)
        {
            for (size_t j = 0; j < NODE_PER_CELL; ++j)
                out << SEP << at(i, j);
            out << std::endl;
        }
    }

    void read(std::istream& in) override
    {
        size_t n_node, n_face, n_cell, n_zone;
        in >> n_node >> n_face >> n_cell >> n_zone;

        m_nn = n_node;
        m_nc = n_cell;

        allocate_storage();

        for (size_t i = 0; i < n_node; ++i)
        {
            size_t tmp;
            in >> tmp;

            in >> m_x.data[i];
            in >> m_y.data[i];
            in >> m_z.data[i];

            std::string left;
            std::getline(in, left);
        }

        for (size_t i = 0; i < n_face; ++i)
        {
            std::string tmp;
            std::getline(in, tmp);
        }

        for (size_t i = 0; i < n_cell; ++i)
        {
            int tmp;
            in >> tmp;

            double c;
            for (int j = 0; j < 3; ++j)
                in >> c;
            in >> c;

            for (size_t j = 0; j < NODE_PER_CELL; ++j)
                in >> at(i, j);

            std::string left;
            std::getline(in, left);
        }
    }

protected:
    void allocate_storage()
    {
        m_x.data.resize(m_nn);
        m_y.data.resize(m_nn);
        m_z.data.resize(m_nn);
        m_node_par.data.resize(m_nn);
        m_cell_par.data.resize(m_nc);
        m_connect.resize(NODE_PER_CELL * m_nc);
    }

    size_t& at(size_t i, size_t j)
    {
        return m_connect.at(NODE_PER_CELL * i + j);
    }
};

class HeterogeneousMeshLoader : public GRID
{
public:
    HeterogeneousMeshLoader() : GRID(FE_MESH_TYPE::POLY) {}

    void read(std::istream& in) override
    {
        /// TODO
    }

    void write_connectivity(std::ostream& out) override
    {
        /// TODO
    }
};

int main(int argc, char* argv[])
{
    std::string input_path;
    std::string output_path;
    std::string zone_text;
    std::string title;
    std::string node_partition_path;
    std::string cell_partition_path;
    FE_MESH_TYPE composition;
    bool composition_flag = false;
    GRID* loader = nullptr;

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
        else if (!std::strcmp(argv[cnt], "--node-partition"))
            node_partition_path = argv[cnt + 1];
        else if (!std::strcmp(argv[cnt], "--cell-partition"))
            cell_partition_path = argv[cnt + 1];
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
        else
            throw std::invalid_argument("Unrecognized option: \"" + std::string(argv[cnt]) + "\".");

        cnt += 2;
    }

    std::ifstream in1(input_path);
    if (in1.fail())
        return -1;

    std::ifstream in2(node_partition_path);
    if (in2.fail())
        return -2;

    std::ifstream in3(cell_partition_path);
    if (in3.fail())
        return -3;

    std::ofstream out(output_path);
    if (out.fail())
        return -4;

    if (!composition_flag)
        throw std::runtime_error("Composition not specified!");

    switch (composition)
    {
    case FE_MESH_TYPE::TET:
    case FE_MESH_TYPE::HEX:
        loader = new HomogeneousMeshLoader(composition);
        break;
    case FE_MESH_TYPE::POLY:
        loader = new HeterogeneousMeshLoader();
        break;
    default:
        break;
    }

    loader->read(in1);
    in1.close();

    if (!zone_text.empty())
        loader->m_zone_text = zone_text;

    if (!title.empty())
        loader->m_title = title;

    loader->load_node_partition(in2);
    in2.close();

    loader->load_cell_partition(in3);
    in3.close();

    loader->write(out);
    out.close();

    delete loader;
    return 0;
}
