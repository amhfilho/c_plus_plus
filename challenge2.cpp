#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <format>

class Texture 
{
public:
    Texture(SDL_Renderer *renderer, const char* path)
        : texturePtr_(IMG_LoadTexture(renderer, path), SDL_DestroyTexture)
        , width_(0.0f)
        , height_(0.0f)
    {
        SDL_GetTextureSize(texturePtr_.get(), &width_, &height_);
        std::cout << "Texture created" << std::endl;

    } 

    ~Texture() 
    {
        std::cout << "Texture destroyed" << std::endl;
    }

    //Texture(const Texture&) = delete;
    //Texture& operator=(const Texture&) = delete;
    
    SDL_Texture* GetRaw() const { return texturePtr_.get(); }
    float GetWidth() const { return width_; }
    float GetHeight() const { return height_; }


private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texturePtr_;
    float width_;
    float height_;

};

class TextureManager 
{
public:
    TextureManager() = default;
    ~TextureManager() 
    {
        std::cout << "Texture Manager destroyed" << std::endl;
    }

    Texture* Load(SDL_Renderer* renderer, const std::string& id, const std::string& path)
    {
        std::cout << "Trying to find " << id << " texture in the cache" << std::endl;

        auto it = textures.find(id);
        if (it != textures.end())
        {
            std::cout << "Found it! Retrieving from the cache " << std::endl;
            return it->second.get();
        }

        std::cout << "Texture not found. Loading a new texture from image and saving it in cache" << std::endl;

        textures[id] = std::make_unique<Texture>(renderer, path.c_str());

        return textures[id].get();

    }

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};


int main() 
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Critical error. Failed to initialize SDL:" << SDL_GetError() << std::endl;
        return -1; 
    }

    using WindowPtr = std::unique_ptr<SDL_Window, decltype (&SDL_DestroyWindow)>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

    WindowPtr window(SDL_CreateWindow("Modulo 2 - TextureManager RAII", 800, 600, 0), SDL_DestroyWindow);
    RendererPtr renderer(SDL_CreateRenderer(window.get(), NULL), SDL_DestroyRenderer);

    std::cout << "Modulo 2 : TextureManager test" << std::endl;

    {
        std::cout << "Enter narrow scope" << std::endl;
        TextureManager textureManager;


        std::cout << "Loading ship" << std::endl; 
        Texture* shipTex = textureManager.Load(renderer.get(), "spaceship", "spaceship.png");
        std::cout << "Loading alien" << std::endl;
        Texture* alienTex = textureManager.Load(renderer.get(), "alien", "alien_1.png");
        std::cout << "Loading ship again" << std::endl;
        Texture* shipAgain = textureManager.Load(renderer.get(), "spaceship", "spaceship.png");

        if (shipTex == shipAgain)
        {
            std::cout << "Identical pointers!" << std::endl;
        }

        std::cout << "Ending narrow scope block" << std::endl;
    }

    std::cout << "End" << std::endl;
    SDL_Quit();

}