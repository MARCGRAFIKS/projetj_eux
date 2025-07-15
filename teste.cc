#include <stdio.h>
#include <string>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>


// Les constantes de dimention
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

//The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

// La classe de texture
class Texture {
    public:
    Texture();
    ~Texture();
    void free();
    bool loadFile(const std::string& name);
    bool loadPixelsFile(const std::string& name);
    bool loadPixels();
    void render(int x, int y, SDL_Rect* clip = NULL);
    int getWidth();
    int getHeight();

    private:
    SDL_Texture* t_tex;
    SDL_Surface* t_surf;
    void* rawPixels;
    int rawPitch;
    int t_width, t_height;
};

class Tile {
    public:
    Tile(int, int, int);
    void render(SDL_Rect&);
    int getType();
    SDL_Rect getBox();
    private:
    SDL_Rect box;
    int type;
};

bool init();
bool loadMedia(Tile* tile[]);
void close(Tile* tile[]);
bool setTiles(Tile* tiles[]);


// Les variables du fentre
SDL_Window* win = NULL;
SDL_Renderer* rend = NULL;
// les textures
Texture tex1;
SDL_Rect clips[TOTAL_TILE_SPRITES];

Texture::Texture() {
    t_tex = NULL;
    t_surf = NULL;
    t_width = 0;
    t_height = 0;
}
    
Texture::~Texture() {
    free();
}
    
void Texture::free() {
    if(t_tex != NULL) {
        SDL_DestroyTexture(t_tex);
        t_tex = NULL;
        t_width = 0;
        t_height = 0;
    }
    if(t_surf != NULL) {
        SDL_FreeSurface(t_surf);
        t_surf = NULL;
    }
}

bool Texture::loadFile(const std::string& name) {
     if(!loadPixelsFile(name)) {
        printf("Error lors du load de %s\n", name.c_str());
     }else {
        if(!loadPixels()) {
            printf("Error de load texture de %s\n", name.c_str());
        }
     }
   return t_tex != NULL;  
}
    
bool Texture::loadPixelsFile(const std::string& name) {
     free();
     SDL_Surface* surf = IMG_Load(name.c_str());
     if(surf == NULL) {
        printf("Error de load image %s! SDL_GEterror!! %s ", name.c_str(), SDL_GetError());
     }else {
        t_surf = SDL_ConvertSurfaceFormat(surf, SDL_GetWindowPixelFormat(win), 0);
        if(t_surf ==NULL) {
            printf("Error de convert surface %s", SDL_GetError());
        }else {
            t_width = t_surf->w;
            t_height = t_surf->h;
        }
        SDL_FreeSurface(surf);
     }
     return t_surf != NULL;
}
    
bool Texture::loadPixels() {
     if(t_surf == NULL) {
        printf("no pixel loaded!");
     }else {
         SDL_SetColorKey(t_surf, SDL_TRUE, SDL_MapRGB(t_surf->format, 0, 0xFF, 0xFF));
        t_tex = SDL_CreateTextureFromSurface(rend, t_surf);
        if(t_tex == NULL) {
            printf("Error de creation du texture%s\n", SDL_GetError());
        }else {
            t_width = t_surf->w;
            t_height = t_surf->h;
        }
        SDL_FreeSurface(t_surf);
        t_surf = NULL;
     }
    return t_tex != NULL; 
}




void Texture::render(int x, int y, SDL_Rect* clip) {
   SDL_Rect rect = {x, y, t_width, t_height};
   if(clip!=NULL) {
    t_width = clip->w;
    t_height = clip->h;
   }
   SDL_RenderCopy(rend, t_tex, clip, &rect);
}
    
int Texture::getWidth() {
    return t_width;
}
    
int Texture::getHeight() {
    return t_height;
}

Tile::Tile(int x, int y, int tileType) {
    box.x = x;
    box.y = y;
    box.w = TILE_WIDTH;
    box.h = TILE_HEIGHT;
    type = tileType;
}
    
void Tile::render(SDL_Rect& camera) {
   tex1.render(box.x-camera.x, box.y-camera.y, &clips[type]);
}
    
int Tile::getType() {
   return type;
}
    
SDL_Rect Tile::getBox() {
  return box;
}

bool init() {
  bool success = true;
  if(SDL_Init(SDL_INIT_VIDEO)<0) {
    printf("Error load sdl!!\n");
    success = false;
  }else {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Fitre de render échouer!!\n");
    }
    win = SDL_CreateWindow("SDL_Load", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                           SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(win == NULL) {
        printf("Error load window!!\n");
        success = false;
    }else {
        rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(rend == NULL) {
            printf("Error load rend!!\n");
            success = false;
        }else {
            SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
            int flag = IMG_INIT_PNG;
            if((!IMG_Init(flag)) & flag) {
               printf("Error laod img!!\n", IMG_GetError());
               success = false;
            }
        }
    }                       
  }
  return success;
}

bool loadMedia(Tile* tiles[]) {
   bool success = true;
   if(!tex1.loadFile("foto/tiles.png")) {
    printf("Error lors de load png!\n");
    success = false;
   }

   if(!setTiles(tiles)) {
    printf("Echeque de load tile type!\n");
    success = false;
   }

   return success;
}

void close(Tile* tiles[]) {
    for(int i(0); i<TOTAL_TILE_SPRITES; ++i) {
        if(tiles[i] != NULL) {
            delete tiles[i];
            tiles[i] = NULL;
        }
    }
  tex1.free();
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  win = NULL;
  rend = NULL;
  IMG_Quit();
  SDL_Quit();
}

bool setTiles(Tile* tiles[]) {
	bool tilesLoad = true;
	int x = 0, y = 0;
	std::ifstream map("foto/lazy.map");
	if(map.fail()) {
		printf("error to load map file!\n");
		tilesLoad = false;
	}else {
		for(int i(0); i< TOTAL_TILES; ++i) {
          int tileType = -1;
		  map >> tileType;
		  if(map.fail()) {
			printf("Error loade tile\n");
			tilesLoad = false;
			break;
		  }
		  if((tileType>=0) && (tileType < TOTAL_TILE_SPRITES)) {
			tiles[i] = new Tile(x, y, tileType);
		  }else {
			printf("Error loading map: invalid tile type at %d!", i);
			tilesLoad = false;
			break;
		  }

		  x += TILE_WIDTH;
		  if(x>= LEVEL_WIDTH) {
			x = 0;
			y += TILE_HEIGHT;
		  }
		}
		if(tilesLoad) {
			clips[ TILE_RED ].x = 0;
			clips[ TILE_RED ].y = 0;
			clips[ TILE_RED ].w = TILE_WIDTH;
			clips[ TILE_RED ].h = TILE_HEIGHT;

			clips[ TILE_GREEN ].x = 0;
			clips[ TILE_GREEN ].y = 80;
			clips[ TILE_GREEN ].w = TILE_WIDTH;
			clips[ TILE_GREEN ].h = TILE_HEIGHT;

			clips[ TILE_BLUE ].x = 0;
			clips[ TILE_BLUE ].y = 160;
			clips[ TILE_BLUE ].w = TILE_WIDTH;
			clips[ TILE_BLUE ].h = TILE_HEIGHT;

			clips[ TILE_TOPLEFT ].x = 80;
			clips[ TILE_TOPLEFT ].y = 0;
			clips[ TILE_TOPLEFT ].w = TILE_WIDTH;
			clips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

			clips[ TILE_LEFT ].x = 80;
			clips[ TILE_LEFT ].y = 80;
			clips[ TILE_LEFT ].w = TILE_WIDTH;
			clips[ TILE_LEFT ].h = TILE_HEIGHT;

			clips[ TILE_BOTTOMLEFT ].x = 80;
			clips[ TILE_BOTTOMLEFT ].y = 160;
			clips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
			clips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

			clips[ TILE_TOP ].x = 160;
			clips[ TILE_TOP ].y = 0;
			clips[ TILE_TOP ].w = TILE_WIDTH;
			clips[ TILE_TOP ].h = TILE_HEIGHT;

			clips[ TILE_CENTER ].x = 160;
			clips[ TILE_CENTER ].y = 80;
			clips[ TILE_CENTER ].w = TILE_WIDTH;
			clips[ TILE_CENTER ].h = TILE_HEIGHT;

			clips[ TILE_BOTTOM ].x = 160;
			clips[ TILE_BOTTOM ].y = 160;
			clips[ TILE_BOTTOM ].w = TILE_WIDTH;
			clips[ TILE_BOTTOM ].h = TILE_HEIGHT;

			clips[ TILE_TOPRIGHT ].x = 240;
			clips[ TILE_TOPRIGHT ].y = 0;
			clips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
			clips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

			clips[ TILE_RIGHT ].x = 240;
			clips[ TILE_RIGHT ].y = 80;
			clips[ TILE_RIGHT ].w = TILE_WIDTH;
			clips[ TILE_RIGHT ].h = TILE_HEIGHT;

			clips[ TILE_BOTTOMRIGHT ].x = 240;
			clips[ TILE_BOTTOMRIGHT ].y = 160;
			clips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
			clips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
		}
	}
	map.close();
	return tilesLoad;
}

int main(int argc, char* argv[]) {
    if(!init()) {
        printf("Error init:\n");
    } else {
             Tile* tiles[TOTAL_TILES];
        if(!loadMedia(tiles)) {
            printf("Error load media!\n");
        }else {
            bool run = false;
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            while(!run) {
                SDL_Event ev;
                while(SDL_PollEvent(&ev)) {
                    if(ev.type == SDL_QUIT) {
                        run = true;
                    }
                }

                SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(rend);
                for(int i(0); i< TOTAL_TILES; ++i) {
                    tiles[i]->render(camera);
                }
                SDL_RenderPresent(rend);
            }
        }
        close(tiles);
    }    
    return 0;
}
