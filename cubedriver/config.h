#pragma once

#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace bpt=boost::property_tree;

namespace Config{

struct Cursor{
    int facelet=-1;
    int camera=-1;
    int x=-1;
    int y=-1;
    int radius=4;
    
    void load(bpt::ptree const &pt);
    void save(bpt::ptree &pt)const;
};
    
struct Camera{
    std::string dev;
    boost::optional<int> exposureAbsolute; // empty = auto
    boost::optional<int> whiteBalanceTemperature; // empty = auto
    boost::optional<int> gain;
    
    void load(bpt::ptree const &pt);
    void save(bpt::ptree &pt)const;
};


struct Root{
    int                 threads;
    std::string         serDev;
    char                solveSpeed;
    char                calSpeed;
    unsigned long       targetMS;
    
    std::vector<Camera> cameras;
    std::vector<Cursor> cursors;
    
    int                 xres;
    int                 yres;
    int                 previewScale;
    
    void load(bpt::ptree const &pt);
    void save(bpt::ptree &pt)const;
};

Root load(std::string const &filename);
void save(std::string const &filename, Root const &root);
    
}