#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <sstream>
#include <fstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

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

class Texture {
	public:
	Texture();
	~Texture();
	bool loadFile(const std::string& name);
	bool loadPixelsFile(const std::string& name);
	bool loadPixels();
	#if defined(SDL_MAJOR_VERSION)
	bool loadRenderText(const std::string& name);
	#endif
	bool createBlank(int, int);
	void free();
	void render(int x, int y, SDL_Rect* clip = NULL);
	int getWidth();
	int getHeight();
	Uint32* getPixels32();
	Uint32 getPixel32(Uint32 x, Uint32 y);
	Uint32 getPitch();
	Uint32 mapRGBA(Uint32, Uint32, Uint32, Uint32);
	void copyRawPixels32(void* pixels);
	bool lockTexture();
	bool unlockTexture();
	private:
	SDL_Texture* t_tex;
	SDL_Surface* t_surf;
	void* rawPixels;
	int t_rawPitch;
	int t_width;
	int t_height;
};

// classe de timer
class LTimer {
	public:
	LTimer();
	void start();
    void stop();
	void pause();
	void unpause();
	Uint32 getTicks();
	bool isStarted();
	bool isPaused();

	private:
	Uint32 startTicks;
	Uint32 pausedTicks;
	bool paused;
	bool started;
};

// classe de tile
class Tile {
	public:
	Tile(int, int, int);
	void render(SDL_Rect& camera);
	int getType();
	SDL_Rect getBox();

	private:
	SDL_Rect box;
	int type;
};

// classe de dot
class Dot {
	public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 640;

	Dot(); 
	void handLeEvent(SDL_Event& e);
	void move(float timeStep);
	void move(Tile *tiles[], float timeStep);
	void setCamera(SDL_Rect& camera);
	void render();
	void render(SDL_Rect& camera);
	private:
	SDL_Rect box;
	float posX, posY;
	float velX, velY;
};

// base de dataStream
class DataDtream {
	public:
	DataDtream();
	bool loadMedia();
	void free();
	void* getBuffer();

	private:
	SDL_Surface* image[4];
	int currentImage;
	int delayFrames;
};

// les fonctions de mains
bool init();
bool loadMedia();
bool loadMedia(Tile* tiles[]);
void close();
void close(Tile* tiles[]);
bool checkCollision(SDL_Rect a, SDL_Rect b);
bool touchesWall(SDL_Rect box, Tile* tiles[]);
bool setTiles(Tile* tiles[]);

// Les variables du fentre
SDL_Window* win = NULL;
SDL_Renderer* rend = NULL;
Texture tex;
Texture tex2;
Texture tileTex;
SDL_Rect clips[TOTAL_TILE_SPRITES];
DataDtream dataStream;

Texture::Texture() {
 t_surf = NULL;
 t_tex = NULL; 
 t_rawPitch= 0;
 t_width = 0;
 t_height = 0;

}
	
Texture::~Texture() {
   free(); 
}
	
bool Texture::loadFile(const std::string& name) {
  if(!loadPixelsFile(name)) {
	printf("Failed to load pixels %s\n", SDL_GetError());
  }else {
	if(!loadPixels()) {
		printf("Failed to texture from pixels %s", SDL_GetError());
	}
  }
  return t_tex != NULL;
}
	
bool Texture::loadPixelsFile(const std::string& name) {
    free();
	SDL_Surface* surf = IMG_Load(name.c_str());
	if(surf == NULL) {
		printf("to load image %s! SDL get error %s", name.c_str(), SDL_GetError());
	}else {
		t_surf = SDL_ConvertSurfaceFormat(surf, SDL_GetWindowPixelFormat(win), 0);
		if(t_surf == NULL) {
			printf("to convert loaded surfce %s", SDL_GetError());
		}else{
			t_width = surf->w;
		    t_height = surf->h;
		}
		SDL_FreeSurface(surf);
	}
	return t_surf != NULL;
}
	
bool Texture::loadPixels() {
   if(t_surf == NULL) {
	printf("no pixels loaded!");
   }else {
	SDL_SetColorKey(t_surf, SDL_TRUE, SDL_MapRGB(t_surf->format, 0, 0xFF, 0xFF));
	t_tex = SDL_CreateTextureFromSurface(rend, t_surf);
	if(t_tex == NULL) {
		printf("to create texture %s\n", SDL_GetError());
	}else {
		t_width = t_surf->w;
		t_height = t_surf->h;
	}
	SDL_FreeSurface(t_surf);
	t_surf = NULL;
   }
   return t_tex != NULL;
}
	
#if defined(SDL_MAJOR_VERSION)
bool Texture::loadRenderText(const std::string& name) {
    free();
	SDL_Surface* surf = NULL;
	if(surf != NULL){
     t_tex = SDL_CreateTextureFromSurface( rend, surf );
	 if(t_tex == NULL) {
		printf("to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
	 }else {
		t_width = surf->w;
		t_height = surf->h;
	 }
	 SDL_FreeSurface(surf);
	}else {
		printf("error to load text %s, TTF get error %s\n", name.c_str(), SDL_GetError());
	} 
	return t_tex != NULL;
}
#endif
	
bool Texture::createBlank(int width, int height) {
	//efacer les données existants
    free();
	// texture initialiser
	t_tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if(t_tex == NULL) {
		printf("to create stream%s\n", SDL_GetError());
	}else {
		t_width = width;
		t_height = height;
	}
    return t_tex;
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
	
void Texture::render(int x, int y, SDL_Rect* clip) {
    SDL_Rect rect = {x, y, t_width, t_height};
	 if(clip != NULL) {
		t_width = t_surf->w;
		t_height = t_surf->h;
	 }
	 SDL_RenderCopy(rend, t_tex, clip, &rect);
}
	
int Texture::getWidth() {
 return t_width;
}
	
int Texture::getHeight() {
  return t_height;
}
	
Uint32* Texture::getPixels32() {
   Uint32* pixels = NULL;
   if(t_surf != NULL) {
	pixels = static_cast<Uint32*>(t_surf->pixels);
   }
   return pixels;
}
	
Uint32 Texture::getPixel32(Uint32 x, Uint32 y) {
  Uint32* pixels = static_cast<Uint32*>(t_surf->pixels);
  return pixels[(y*getPitch())+x];
}
	
Uint32 Texture::getPitch() {
    Uint32 pitch = 0;
   if(t_surf != NULL) {
	pitch = t_surf->pitch/4;
   }
   return pitch;
}
	
Uint32 Texture::mapRGBA(Uint32 r, Uint32 g, Uint32 b, Uint32 a) {
  Uint32 pixel = 0;
  if(t_surf != NULL) {
	pixel = SDL_MapRGBA(t_surf->format, r, g, b, a);
  }
  return pixel;
}
	
void Texture::copyRawPixels32(void* pixels) {
  if(rawPixels != NULL) {
	memcpy(rawPixels, pixels, t_rawPitch*t_height);
  }
}
	
bool Texture::lockTexture() {
   bool success = true;
   if(rawPixels != NULL) {
	printf("texture is already  to lock\n");
     success = false;
   }else {
	if(SDL_LockTexture(t_tex, NULL, &rawPixels, &t_rawPitch) != 0) {
       printf("error to lock texxture! %s\n", SDL_GetError());
	   success = false;
	}
   }
   return success;
}
	
bool Texture::unlockTexture() {
  bool success = true;
   if(rawPixels == NULL) {
	printf("texture is nt locked!\n");
	success = false;
   }else {
	SDL_UnlockTexture(t_tex);
	rawPixels = NULL;
	t_rawPitch = 0;
   }
   return success;
}

Tile::Tile(int x, int y, int tileType) {
  box.x = x;
  box.y = y;
  box.w = TILE_WIDTH;
  box.h = TILE_HEIGHT;
  type = tileType;
}
	
void Tile::render(SDL_Rect& camera) {
   if(checkCollision(camera, box)) {
	tileTex.render(box.x-camera.x, box.y-camera.y, &clips[type]);
   }
}
	
int Tile::getType() {
   return type;
}
	
SDL_Rect Tile::getBox() {
   return box;
}

DataDtream::DataDtream() {
  image[0] = NULL;
  image[1] = NULL;
  image[2] = NULL;
  image[3] = NULL;
  currentImage = 0;
  delayFrames = 4;
}	

bool DataDtream::loadMedia() {
	bool success = true;
	for(int i(0); i < 4; ++i) { 
       std::stringstream path;
       path << "foto/foo_walk_" << i << ".png";

	   SDL_Surface* surf = IMG_Load(path.str().c_str());
	   if(surf == NULL) {
		printf("to load %s! SDL_Error %s\n", path.str().c_str(), IMG_GetError());
		success = false;
	   }else {
		  image[i] = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA8888, 0);
	   }
	   SDL_FreeSurface(surf);
	}
	return success;
}
	
void DataDtream::free() {
    for(int i(0); i< 4; ++i) {
		SDL_FreeSurface(image[i]);
		image[i] = NULL;
	}
}
	
void* DataDtream::getBuffer() {
    --delayFrames;
	if(delayFrames == 0) {
		++currentImage;
		delayFrames = 10;
	}

	if(currentImage == 4) {
		currentImage = 0;
	}
	return image[currentImage]->pixels;
}

LTimer::LTimer() {
   startTicks = 0;
   pausedTicks = 0;

   paused = false;
   started = false;
}
	
void LTimer::start() {
   started = true;
   paused = false;
   startTicks = SDL_GetTicks();
   pausedTicks = 0;
}
    
void LTimer::stop() {
   started = false;
   paused = false;
   startTicks = 0;
   pausedTicks = 0;
}
	
void LTimer::pause() {
  if(started && !paused) {
	paused = true;

	pausedTicks = SDL_GetTicks() - startTicks;
	startTicks = 0;
  }
}
	
void LTimer::unpause() {
	if(started && paused) {
		paused = false;
		startTicks = SDL_GetTicks() - pausedTicks;
		pausedTicks = 0;
	}

}
	
Uint32 LTimer::getTicks() {
   Uint32 time = 0;
   if(started) {
	  if(paused) {
	    	time = pausedTicks;
	    }else {
	    	time = SDL_GetTicks() - startTicks;
    	}
   }
   return time;
}
	
bool LTimer::isStarted() {
	return started;

}
	
bool LTimer::isPaused() {
  return paused && started;
}

Dot::Dot() {
 posX = 0;
 posY= 0;
 velX = 0;
 velY = 0;
}
	
void Dot::handLeEvent(SDL_Event& e) {
   if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
      switch(e.key.keysym.sym) {
		case SDLK_UP: velY -= DOT_VEL; break;
		case SDLK_DOWN: velY += DOT_VEL; break;
		case SDLK_LEFT: velX -= DOT_VEL; break;
		case SDLK_RIGHT: velX += DOT_VEL; break;
	  }
   }else
    if(e.type == SDL_KEYUP && e.key.repeat == 0) {
      switch(e.key.keysym.sym) {
		case SDLK_UP: velY += DOT_VEL; break;
		case SDLK_DOWN: velY -= DOT_VEL; break;
		case SDLK_LEFT: velX += DOT_VEL; break;
		case SDLK_RIGHT: velX -= DOT_VEL; break;
	  }
   }
}
	 
void Dot::move(float timeStep) {
  posX += velX * timeStep;
  if(posX < 0) {
	posX = 0;
  }else
  if(posX >SCREEN_WIDTH - DOT_WIDTH) {
	posX = SCREEN_WIDTH - DOT_WIDTH;
  }

  posY += velY * timeStep;
  if(posY < 0) {
	posY = 0;
  }else
  if(posY >SCREEN_HEIGHT - DOT_HEIGHT) {
	posY = SCREEN_HEIGHT - DOT_HEIGHT;
  }
}

void Dot::move(Tile *tiles[], float timeStep) {
   box.x += velX * timeStep;
  if((box.x < 0) || (box.x + DOT_WIDTH > LEVEL_WIDTH) || touchesWall(box, tiles)) {
	box.x += velX * timeStep;
  }

  box.y += velY * timeStep;
  if((box.y < 0) || (box.y + DOT_HEIGHT > LEVEL_HEIGHT) || touchesWall(box, tiles)) {
	box.y += velY * timeStep;
  }
}

void Dot::setCamera(SDL_Rect& camera) {
	camera.x = (box.x + DOT_WIDTH/2) - SCREEN_WIDTH/2;
	camera.y = (box.y + DOT_HEIGHT/2) - SCREEN_HEIGHT/2;

	if(camera.x < 0) {
		camera.x = 0;
	}
	if(camera.y < 0) {
		camera.y = 0;
	}
	if(camera.x > LEVEL_WIDTH - camera.w) {
       camera.x = LEVEL_WIDTH - camera.w;
	}
	if(camera.y > LEVEL_HEIGHT - camera.h) {
       camera.y = LEVEL_HEIGHT - camera.h;
	}
}
	
void Dot::render() {
   tex2.render((int)posX, (int)posY);
}

void Dot::render(SDL_Rect& camera) {
   tex2.render(box.x-camera.x, box.y-camera.y);
}

bool init(){
   bool success = true;
   if(SDL_Init(SDL_INIT_VIDEO) < 0) {
	printf("SDL could not initialision! SDL Error: %s\n", SDL_GetError());
	success = false;
   }else {
	  if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		printf("filtre de texture echoué\n");
	  }

	  srand(SDL_GetTicks());

	  win = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( win == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}else {
            //Create renderer for window
			rend = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( rend == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( rend, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		} 
   }
   return success;
}

bool loadMedia() {
   bool success = true;
   if(!tex.createBlank(64, 205)) {
	printf("èchouer de create stream\n");
	success = false;
   }
   if(!dataStream.loadMedia()) {
	 printf("èchouer to load stream\n");
	 success = false;
   }
   if(!tex2.loadFile("foto/dot.bmp")) {
	printf("Load tex2 failed\n");
	success = false;
   }
   return success;
}

bool loadMedia(Tile* tiles[]) {
	bool success = true;
	if(!tileTex.loadFile("foto/tiles.png")) {
		printf("Failed to load tiles \n");
		success = false;
	}
	if(!setTiles(tiles)) {
       printf("load tile set! \n");
	   success = false;
	}
	return success;
}

void close() {
   tex.free();
   dataStream.free();
   tex2.free();

   SDL_DestroyRenderer(rend);
   SDL_DestroyWindow(win);
   win = NULL;
   rend = NULL;
   IMG_Quit();
   SDL_Quit();
}

void close(Tile* tiles[]) {
   for(int i(0); i<TOTAL_TILE_SPRITES; ++i) {
	if( tiles[i] != NULL) {
		delete tiles[i];
		tiles[i] = NULL;
	}
   }	
   tex.free();
   dataStream.free();
   tex2.free();

   SDL_DestroyRenderer(rend);
   SDL_DestroyWindow(win);
   win = NULL;
   rend = NULL;
   IMG_Quit();
   SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	if(bottomA <= topB) {
		return false;
	}
	if(topA>= bottomB) {
		return false;
	}
	if(rightA <= leftB) {
		return false;
	}
	if(leftA >= rightB) {
		return false;
	}
	return true;
}

bool touchesWall(SDL_Rect box, Tile* tiles[]) {
	for(int i(0); i<TOTAL_TILES; ++i) {
		if((tiles[i]->getType() >= TILE_CENTER)&& (tiles[i]->getType() <=TILE_TOPLEFT)) {
			if(checkCollision(box, tiles[i]->getBox())) {
				return true;
			}
		}
	}
	return false;
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
		printf("Fziled to initialiser!\n");
	}else {
		Tile* tileSet[TOTAL_TILES];
		if(!loadMedia() && !loadMedia(tileSet)) {
			printf("Failed to loac media!\n");
		} else {
			bool run = false;
			SDL_Event e;
			Dot dot;
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
			LTimer setpTimer;
			while(!run) {
				while(SDL_PollEvent(&e)) {
					if(e.type == SDL_QUIT) {
						run = true;
					}
					dot.handLeEvent(e);
				}
				float timeStep = setpTimer.getTicks()/1000.f;
				dot.move(tileSet, timeStep);
				dot.setCamera(camera);
				setpTimer.start();
				SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(rend);
				for(int i(0); i<TOTAL_TILES; ++i) {
					tileSet[i]->render(camera);
				}

				tex.lockTexture();
				tex.copyRawPixels32(dataStream.getBuffer());
                tex.unlockTexture();

				tex.render((SCREEN_WIDTH-tex.getWidth())/2, (SCREEN_HEIGHT-tex.getHeight())/2);
				dot.render(camera);
				SDL_RenderPresent(rend);
			}
		}
		close(tileSet);
	}
	close();
	return 0;
}
