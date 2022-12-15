#include <iostream>
#include "AppWindow.h"
#include "Examples.h"
#include "Functions.h"
#include "imgui.h"
#include "Texture.h"
#include "MImageCalculator.h"


float MyAwesomeFunction(SDL_Point arg)
{
	int x_size = 800;
	int y_size = 600;

	const SDL_Point circlePos1{ x_size / 3, y_size / 3 * 1.2 };
	const float circleRadius1 = std::max(x_size, y_size) / 4;
	const SDL_Point circlePos2{ x_size / 3 * 2, y_size / 3 * 1.2 };
	const float circleRadius2 = std::max(x_size, y_size) / 4;


	/////////////////
	const SDL_Point circlePos3{ x_size / 6 * 1.6, y_size / 3 };
	const float circleRadius3 = std::max(x_size, y_size) / 6;

	const SDL_Point circlePos4{ x_size / 3 * 2, y_size / 3 * 2 };
	const float circleRadius4 = std::max(x_size, y_size) / 6;

	const SDL_Point circlePos5{ x_size / 6 * 2, y_size * 0.7 };
	const float circleRadius5 = std::max(x_size, y_size) / 6;

	const SDL_Point circlePos6{ x_size / 5 * 3.7, y_size * 0.3 };
	const float circleRadius6 = std::max(x_size, y_size) / 6;
	//////////////////


	float c1 = Functions::Circle(circlePos1, circleRadius1, arg);
	float c2 = Functions::Circle(circlePos2, circleRadius2, arg);


	///////////
	float c3 = Functions::Circle(circlePos3, circleRadius3, arg);
	float c4 = Functions::Circle(circlePos4, circleRadius4, arg);
	float c5 = Functions::Circle(circlePos5, circleRadius5, arg);
	float c6 = Functions::Circle(circlePos6, circleRadius6, arg);
	///////////


	//float res = Functions::ROr(c1, c2);
	float res1 = Functions::RAnd(c1, c2);
	float res2 = Functions::ROr(c3, c4);
	float res3 = Functions::ROr(c5, c6);

	float res4 = Functions::ROr(res1, res2);
	float res = Functions::ROr(res3, res4);

	return res;
}

class Window: public AppWindow
{


public:
	Window(SDL_Point windowSize) :
		AppWindow(windowSize),
		texture(GetRenderer(), windowSize),
		calculator(&MyAwesomeFunction)
		
	{
		ComputeFunction();
	}
	
	void Render() override
	{
		texture.Render();
	}
	
	void RenderGui() override
	{
		ImGui::Begin("MyWindow");
		ImGui::Text("Mouse position");
		ImGui::Text("\t%d, %d", mousePosition.x, mousePosition.y);
		ImGui::End();
	}
	
	void ProcessEvent(const SDL_Event& e) override
	{
		if (e.type == SDL_MOUSEMOTION)
		{
			mousePosition.x = e.motion.x;
			mousePosition.y = e.motion.y;
			pixelData = calculator.GetPixelData(mousePosition);

		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			if (e.button.button == SDL_BUTTON_RIGHT)
			{
				SDL_Point startPoint;
				startPoint.x = e.button.x;
				startPoint.y = e.button.y;
				GradientDescent(startPoint, 2);
			}
		}
	}

	void ComputeFunction()
	{
		std::vector<std::vector<MImagePixelData>> data = calculator.GetSpaceData(texture.GetSize());
		for (size_t i = 0; i < data.size(); ++i)
		{
			for (size_t j = 0; j < data[i].size(); ++j)
			{
				MImagePixelData mimageData = data[i][j];
				uint8_t colorValue = (mimageData.nx + 1.f) * 127.f;
				if (mimageData.zone == FunctionZone::Positive)
					texture.SetPixel(SDL_Point{ (int)i, (int)j }, SDL_Color{ colorValue, 0, 0, 255 });
				else if (mimageData.zone == FunctionZone::Negative)
					texture.SetPixel(SDL_Point{ (int)i, (int)j}, SDL_Color{ 0, 0, colorValue, 255 });
			}
		}
		texture.UpdateTexture();
	}
	


	void GradientDescent(SDL_Point startPoint, int step, SDL_Color pathColor = { 0, 255, 0, 255 })
	{
		while (startPoint.x >= 0 && startPoint.x < texture.GetSize().x &&
			startPoint.y >= 0 && startPoint.y < texture.GetSize().y)
		{
			SDL_Color newColor = texture.GetColor({ startPoint.x, startPoint.y });
			if (newColor.r == pathColor.r &&
				newColor.g == pathColor.g &&
				newColor.b == pathColor.b &&
				newColor.a == pathColor.a)
			{
				break;
			}

			texture.SetPixel({ startPoint.x, startPoint.y }, pathColor);

			MImagePixelData data = calculator.GetPixelData({ startPoint.x, startPoint.y });
			startPoint.x -= step * data.nx;
			startPoint.y += step * data.ny;
		}
		texture.UpdateTexture();
	}


	Texture texture;
	MImageCalculator calculator;
	SDL_Point mousePosition;
	MImagePixelData pixelData;
};


int main(int argc, char** argv)
{
	Window window({800, 600});
	
	window.Show();
	
	return 0;
}