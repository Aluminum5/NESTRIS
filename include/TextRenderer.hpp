#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Graphics/Font.hpp>
#include<vector>
#include<memory>

class TextRenderer {
public:
	TextRenderer(sf::Font const& _font);
	void addText(sf::Text const& text);
	void clear();
	void draw(sf::RenderTarget& render_target);
	std::vector<sf::Text> const& getTexts() const;
private:
	sf::RenderTarget* render_target;
	sf::Font const& font;
	std::vector<sf::Text> text_vector;
};