#include "config.h"

namespace Config{



void Cursor::load(bpt::ptree const &pt){
    facelet = pt.get<int>("facelet");
    camera  = pt.get<int>("camera");
    x       = pt.get<int>("x");
    y       = pt.get<int>("y");
    radius  = pt.get<int>("radius", 4);
}
void Cursor::save(bpt::ptree &pt)const{
    pt.put("facelet", facelet);
    pt.put("camera",  camera);
    pt.put("x",       x);
    pt.put("y",       y);
    pt.put("radius",  radius);
}
    
void Camera::load(bpt::ptree const &pt){
    dev = pt.get<std::string>("dev");
    exposureAbsolute = pt.get_optional<int>("exposureAbsolute");
    whiteBalanceTemperature = pt.get_optional<int>("whiteBalanceTemperature");
    gain = pt.get_optional<int>("gain");
}
void Camera::save(bpt::ptree &pt)const{
    pt.put("dev", dev);
    if(exposureAbsolute){
        pt.put("exposureAbsolute", *exposureAbsolute);
    }
    if(whiteBalanceTemperature){
        pt.put("whiteBalanceTemperature", *whiteBalanceTemperature);
    }
    if(gain){
        pt.put("gain", *gain);
    }
}
    

void Root::load(bpt::ptree const &pt){
    threads    = pt.get<int>("threads", 2 );    
    serDev     = pt.get<std::string>("serDev");
    solveSpeed = pt.get<char>("solveSpeed", '3');
    calSpeed   = pt.get<char>("calSpeed",   '3');
    targetMS   = pt.get<unsigned long>("targetMS",   980);
    
    xres        = pt.get<int>("xres", 160 );
    yres        = pt.get<int>("yres", 120 );
    previewScale= pt.get<int>("previewScale", 2);
    
    for(auto &&v : pt.get_child("cameras", bpt::ptree())){
        cameras.push_back(Camera());
        cameras.back().load( v.second );
    }
    for(auto &&v : pt.get_child("cursors", bpt::ptree())){
        cursors.push_back(Cursor());
        cursors.back().load( v.second );
    }
}
void Root::save(bpt::ptree &pt)const{
    pt.put("threads",    threads);
    pt.put("serDev",     serDev);
    pt.put("solveSpeed", solveSpeed);
    pt.put("calSpeed",   calSpeed);
    pt.put("targetMS",   targetMS);

    pt.put("xres",         xres);
    pt.put("yres",         yres);
    pt.put("previewScale", previewScale);
    
    bpt::ptree &pt_cameras = pt.put_child("cameras", bpt::ptree());        
    for(auto &&camera : cameras){
        bpt::ptree &pt_camera = pt_cameras.add_child("camera", bpt::ptree());        
        camera.save(pt_camera);
    }
    bpt::ptree &pt_cursors = pt.put_child("cursors", bpt::ptree());        
    for(auto &&cursor : cursors){
        bpt::ptree &pt_cursor = pt_cursors.add_child("cursor", bpt::ptree());        
        cursor.save(pt_cursor);
    }
}

Root load(std::string const &filename){
    bpt::ptree pt;
    read_info(filename, pt);    
    Config::Root root;
    root.load(pt);
    return root; 
}

void save(std::string const &filename, Root const &root){
    bpt::ptree pt;
    root.save(pt);
    write_info(filename, pt);
}

}
