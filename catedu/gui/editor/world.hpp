struct Building
{
    int floors;
    int x, y;
};

struct World
{
    Building buildings[100];
    int num_buildings;

    static World create();
    void add_building(int floors, int x, int y);
    void remove_building(int x, int y);
};