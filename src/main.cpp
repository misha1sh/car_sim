#include "main_window.h"
#include "rasterize/roads_extractor.h"

#include <QApplication>
//#include <proj_api.h>

#include <osmium/geom/mercator_projection.hpp>
#include <osmium/geom/haversine.hpp>
#include <iostream>
#include "utils/UTM.h"

#include "rasterize/projector.h"

double sqr(double x) {
    return x * x;
}

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
//    RoadsExtractor extractor("../data/map.osm");
////    extractor.ExtractRoads();
//    osmium::geom::Projection().operator()(osmium::Location);
////    osmium::geom::Coordinates
//    double x, y;
//    char* zone = new char[100];
//    UTM::LLtoUTM(55.75307, 37.60046, x, y, zone);
//
//    auto pj_longlat = pj_init_plus("+init=epsg:4326" );
//    auto pj_utm = pj_init_plus("+init=epsg:23030" );
//
//
//    double  x2, y2;
//    int p = pj_transform( pj_utm, pj_longlat, 55.75307, 37.60046, &x2, &y2, NULL );
    using namespace osmium::geom;
    // lat lon
    const auto center = Coordinates{37.60046, 55.75307};
    const auto a = Coordinates{37.65046, 55.79307};
    const auto b = Coordinates{37.50046, 55.76307};
    std::cerr << a.valid() << " " << b.valid() << std::endl;
//
//    const auto aa = Coordinates{a.x - center.x, a.y - center.y};
//    const auto bb = Coordinates{b.x - center.x, b.y - center.y};
//
//    const auto a_proj = lonlat_to_mercator(aa);
//    const auto b_proj = lonlat_to_mercator(bb);
//
    double dist = osmium::geom::haversine::distance(a, b);
//    double dist3 = osmium::geom::haversine::distance(aa, bb);


//
//
//    const auto centerp = proj_coord(center.y, center.x, 0, 0);
//    const auto ap = proj_coord(a.y, a.x, 0, 0);
//    const auto bp = proj_coord(b.y, b.x, 0, 0);
//    const auto cener_proj = proj_trans(P, PJ_FWD, centerp);
//    const auto a_proj = proj_trans(P, PJ_FWD, ap);
//    const auto b_proj = proj_trans(P, PJ_FWD, bp);
//
//
    Projector projector(Coord{center});
    const auto a_proj = projector.project(Coord{a});
    const auto b_proj = projector.project(Coord{b});
    double dist2 = sqrt(sqr(a_proj.xy.x - b_proj.xy.x) + sqr(a_proj.xy.y - b_proj.xy.y));

    return 0;
}