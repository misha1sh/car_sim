import osmium as o
import sys
from shapely.geometry import LineString, Polygon
import shapefile
from utm import from_latlon, to_latlon

def expand_line(line, width):
    line_xy = [from_latlon(point[0], point[1], 37, 'U')[:2] for point in line]

    line = LineString(line_xy)
    polys = line.buffer(width, 2)
    if type(polys) == Polygon:
        polys = [polys]


    coords = []
    for poly in polys:
        cc = zip(*poly.exterior.coords.xy)
        coords.append(list(to_latlon(c[0], c[1], 37, 'U') for c in cc))

    return coords

LANE_WIDTH = 4.

class RoadWriter(o.SimpleHandler):
    def __init__(self):
        super(RoadWriter, self).__init__()
        self.nodes = {}
        self.polys = []

    def way(self, w):
        # https://wiki.openstreetmap.org/wiki/Key:highway?uselang=en
        if 'highway' in w.tags and w.tags['highway'] in {'primary', 'secondary', 'tertiary', 'motorway', 'trunk',
                                                         'unclassified',
                                                         'primary_link', 'secondary_link', 'tertiary_link', 'motorway_link', 'trunk_link'}:
            line = []
            for n in w.nodes:
                if n.ref in self.nodes:
                    line.append(self.nodes[n.ref])
                else:
                    return
            if len(line) <= 1:
                return


            width = LANE_WIDTH
            if 'lanes' in w.tags:
                try:
                    width = LANE_WIDTH * float(w.tags['lanes'])
                finally:
                    pass

            width = 0.1

            polys = expand_line(line, width / 2)
            self.polys += [(poly, w.id) for poly in polys]

    def node(self, node):
        self.nodes[node.id] = (node.location.lat, node.location.lon)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python osm2road.py <infile> <outfile>")
        sys.exit(-1)


    # go through the ways to find all relevant nodes
    ways = RoadWriter()
    ways.apply_file(sys.argv[1])

    writer = shapefile.Writer(sys.argv[2], shapefile.POLYGON)
    writer.field('name', 'C', '150')

    for (poly, id) in ways.polys:
        writer.poly([poly])
        writer.record("https://www.openstreetmap.org/way/" + str(id))
        # writer.record("https://www.openstreetmap.org/query?lat=%.5flon=%.5f#map=20/%5.f/%.5f" % (poly[0][0], poly[0][1], poly[0][0], poly[0][1]))
        # writer.record("%.4f/%.4f" % (poly[0][0], poly[0][1]))

    writer.close()

# gdal_rasterize -burn 255 -burn 0 -burn 0 -ot Byte -ts 5000 5000 -te 37.500766 55.735902 37.512522 55.740176 -l building-polygon building-polygon.shp mask.tif

# l:~/course/utils/data$ gdal_rasterize -burn 255 -burn 0 -burn 0 -ot Byte -ts 5000 5000 -l res res.shp mask.tif