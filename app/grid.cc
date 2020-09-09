#include <iostream>
#include "../inc/tec.h"

class TetMeshLoader : public TET_GRID
{
public:
    void read(std::istream &in) override
    {
        size_t n_node, n_face, n_cell, n_zone;
        in >> n_node >> n_face >> n_cell >> n_zone;

        m_x.data.resize(n_node);
        m_y.data.resize(n_node);
        m_z.data.resize(n_node);
        m_node_par.data.resize(n_node, 0);
        m_cell_par.data.resize(n_cell, 0);
        m_connect.resize(4*n_cell);

        for(size_t i = 0; i < n_node; ++i)
        {
            size_t tmp;
            in >> tmp;

            in >> m_x.data[i];
            in >> m_y.data[i];
            in >> m_z.data[i];

            size_t n;
            in >> n;
            for(size_t j = 0; j < n; ++j)
                in >> tmp;

            in >> n;
            for(size_t j = 0; j < n; ++j)
                in >> tmp;

            in >> n;
            for(size_t j = 0; j < n; ++j)
                in >> tmp;
        }

        for(size_t i = 0; i < n_face; ++i)
        {
            size_t tmp;

            in >> tmp;
            in >> tmp;

            double cx, cy, cz;
            in >> cx >> cy >> cz;

            double s;
            in >> s;

        }
    }
};

class HexMeshLoader : public HEX_GRID
{
public:
    void read(std::istream &in) override
    {

    }
};

int main(int argc, char *argv[])
{


    return 0;
}