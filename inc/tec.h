#ifndef TECPLOT_H
#define TECPLOT_H

#include <fstream>
#include <string>
#include <vector>
#include <cstddef>

enum class FE_MESH_TYPE : int {TET = 1, HEX = 2, POLY = 3};

template<typename T>
struct DATA_BLOCK
{
    std::string name;
    std::vector<T> data;

    void write(std::ostream &out, size_t nRec1Line, const char &sep)
    {
        size_t i = 0;
        for (const auto &e : data)
        {
            out << sep << e;
            ++i;
            if (i % nRec1Line == 0)
                out << std::endl;
        }
        if (data.size() % nRec1Line != 0)
            out << std::endl;
    }
};

struct HEADER
{
    std::string m_title;
    std::string m_zone_text;
    size_t m_strand;
    size_t m_nn;
    size_t m_nc;
};

class GRID : public HEADER
{
protected:
    FE_MESH_TYPE m_tp;
    DATA_BLOCK<double> m_x;
    DATA_BLOCK<double> m_y;
    DATA_BLOCK<double> m_z;
    DATA_BLOCK<size_t> m_node_par;
    DATA_BLOCK<size_t> m_cell_par;

public:
    GRID(FE_MESH_TYPE tp): m_tp(tp)
    {
        m_x.name = "X";
        m_y.name = "Y";
        m_z.name = "Z";
        m_node_par.name = "NODE_PARTITION";
        m_cell_par.name = "CELL_PARTITION";
    }

    virtual ~GRID() = default;

    virtual void read(std::istream &in) = 0;

    void write(std::ostream &out);

    virtual void write_connectivity(std::ostream &out) = 0;

    void load_node_partition(std::istream &in)
    {
        for(auto &e : m_node_par.data)
            in >> e;
    }

    void load_cell_partition(std::istream &in)
    {
        for(auto &e : m_cell_par.data)
            in >> e;
    }
};

class DATA : public HEADER
{
protected:
    double m_tm;
    FE_MESH_TYPE m_tp;
    std::vector<DATA_BLOCK<double>*> m_var;

public:
    DATA() {}

    virtual void read(std::istream &in) = 0;

    void write(std::ostream &out);

    virtual ~DATA()
    {
        for(auto e: m_var)
            delete e;

        m_var.clear();
    }
};

#endif
