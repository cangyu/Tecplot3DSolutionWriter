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
    DATA_BLOCK<double> m_x;
    DATA_BLOCK<double> m_y;
    DATA_BLOCK<double> m_z;
    DATA_BLOCK<size_t> m_node_par;
    DATA_BLOCK<size_t> m_cell_par;

public:
    GRID()
    {
        m_x.name = "X";
        m_y.name = "Y";
        m_z.name = "Z";
        m_node_par.name = "NODE_PARTITION";
        m_cell_par.name = "CELL_PARTITION";
    }

    virtual void read(std::istream &in) = 0;

    virtual void write(std::ostream &out) = 0;

    void load_partition(std::istream &np_in, std::istream &cp_in);
};

class TET_GRID : public GRID
{
protected:
    std::vector<size_t> m_connect;

public:
    TET_GRID() = default;

    size_t at(size_t i, size_t j) const
    {
        return m_connect.at(4 * i + j);
    }

    size_t &at(size_t i, size_t j)
    {
        return m_connect.at(4 * i + j);
    }

    void write(std::ostream &out) override;
};

class HEX_GRID : public GRID
{
protected:
    std::vector<size_t> m_connect;

public:
    HEX_GRID() = default;

    size_t at(size_t i, size_t j) const
    {
        return m_connect.at(8 * i + j);
    }

    size_t &at(size_t i, size_t j)
    {
        return m_connect.at(8 * i + j);
    }

    void write(std::ostream &out) override;
};

class POLY_GRID : public GRID
{
private:
    size_t n_tet, n_hex, n_pyramid, n_prism; /// Composition of the grid

public:
    POLY_GRID()
    {
        n_tet = n_hex = n_prism = n_pyramid = 0;
    }

    void write(std::ostream &out) override;
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

    ~DATA()
    {
        for(auto e: m_var)
            delete e;

        m_var.clear();
    }
};

#endif
