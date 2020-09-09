#include <stdexcept>
#include "../inc/tec.h"

void GRID::load_partition(std::istream &np_in, std::istream &cp_in)
{
    for(auto &e : m_node_par.data)
    {
        np_in >> e;
    }

    for(auto &e : m_cell_par.data)
    {
        cp_in >> e;
    }
}

void TET_GRID::write(std::ostream &out)
{
    /// File Header
    out << "TITLE=\"" << m_title << "\"" << std::endl;
    out << "FILETYPE=GRID" << std::endl;
    out << "VARIABLES=";
    out << "\"" << m_x.name << "\",";
    out << "\"" << m_y.name << "\",";
    out << "\"" << m_z.name << "\",";
    out << "\"" << m_node_par.name << "\",";
    out << "\"" << m_cell_par.name << "\"" << std::endl;

    /// Zone Record
    out << "ZONE T=\"" << m_zone_text << "\"" << std::endl;
    out << "STRANDID=" << m_strand << std::endl;
    out << "NODES=" << m_nn << std::endl;
    out << "ELEMENTS=" << m_nc << std::endl;
    out << "ZONETYPE=" << "FETETRAHEDRON" << std::endl;
    out << "DATAPACKING=BLOCK" << std::endl;
    out << "VARLOCATION=([1-4]=NODAL,[5]=CELLCENTERED)" << std::endl;

    /// Format param
    static const size_t RECORD_PER_LINE = 10;
    static const char SEP = ' ';

    /// X-Coordinates
    m_x.write(out, RECORD_PER_LINE, SEP);

    /// Y-Coordinates
    m_y.write(out, RECORD_PER_LINE, SEP);

    /// Z-Coordinates
    m_z.write(out, RECORD_PER_LINE, SEP);

    /// Partition of nodes
    m_node_par.write(out, RECORD_PER_LINE, SEP);

    /// Partition of cells
    m_cell_par.write(out, RECORD_PER_LINE, SEP);

    /// Connectivity
    for(size_t i = 0; i < m_nc; ++i)
    {
        for(size_t j = 0; j < 4; ++j)
            out << SEP << at(i, j);
        out << std::endl;
    }
}

void HEX_GRID::write(std::ostream &out)
{
    /// File Header
    out << "TITLE=\"" << m_title << "\"" << std::endl;
    out << "FILETYPE=GRID" << std::endl;
    out << "VARIABLES=";
    out << "\"" << m_x.name << "\",";
    out << "\"" << m_y.name << "\",";
    out << "\"" << m_z.name << "\",";
    out << "\"" << m_node_par.name << "\",";
    out << "\"" << m_cell_par.name << "\"" << std::endl;

    /// Zone Record
    out << "ZONE T=\"" << m_zone_text << "\"" << std::endl;
    out << "STRANDID=" << m_strand << std::endl;
    out << "NODES=" << m_nn << std::endl;
    out << "ELEMENTS=" << m_nc << std::endl;
    out << "ZONETYPE=" << "FEBRICK" << std::endl;
    out << "DATAPACKING=BLOCK" << std::endl;
    out << "VARLOCATION=([1-4]=NODAL,[5]=CELLCENTERED)" << std::endl;

    /// Format param
    static const size_t RECORD_PER_LINE = 10;
    static const char SEP = ' ';

    /// X-Coordinates
    m_x.write(out, RECORD_PER_LINE, SEP);

    /// Y-Coordinates
    m_y.write(out, RECORD_PER_LINE, SEP);

    /// Z-Coordinates
    m_z.write(out, RECORD_PER_LINE, SEP);

    /// Connectivity
    for(size_t i = 0; i < m_nc; ++i)
    {
        for(size_t j = 0; j < 8; ++j)
            out << SEP << at(i, j);
        out << std::endl;
    }
}

void POLY_GRID::write(std::ostream &out)
{
    throw std::runtime_error("Polyhedral grid is NOT supported currently!");
}

void DATA::write(std::ostream &out)
{
    if(m_var.empty())
        throw std::runtime_error("NO data to write out!");

    /// File Header
    out << "TITLE=\"" << m_title << "\"" << std::endl;
    out << "FILETYPE=SOLUTION" << std::endl;
    out << "VARIABLES=\"" << m_var[0]->name << "\"";
    for(size_t i = 1; i < m_var.size(); ++i)
        out << ",\"" << m_var[i]->name << "\"";
    out << std::endl;

    /// Zone Record
    out << "ZONE T=\"" << m_zone_text << "\"" << std::endl;
    out << "STRANDID=" << m_strand << std::endl;
    out << "SOLUTIONTIME=" << m_tm << std::endl;
    out << "NODES=" << m_nn << std::endl;
    out << "ELEMENTS=" << m_nc << std::endl;
    out << "ZONETYPE=";
    switch (m_tp)
    {
    case FE_MESH_TYPE::TET:
       out << "FETETRAHEDRON";
       break;
    case FE_MESH_TYPE::HEX:
        out << "FEBRICK";
        break;
    case FE_MESH_TYPE::POLY:
        throw std::runtime_error("Polyhedral data is NOT supported currently!");
    default:
        break;
    }
    out << std::endl;
    out << "DATAPACKING=BLOCK" << std::endl;
    out << "VARLOCATION=([1-" << m_var.size() << "]=NODAL)" << std::endl;

    /// Format param
    static const size_t RECORD_PER_LINE = 10;
    static const char SEP = ' ';

    for(auto e : m_var)
        e->write(out, RECORD_PER_LINE, SEP);
}
