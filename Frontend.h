#pragma once

class Frontend
{
	Frontend();
	virtual ~Frontend();

	virtual void Update();
	virtual void Draw() const;

};