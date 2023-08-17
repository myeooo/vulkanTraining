#pragma once

struct QueueFamilyindices {
	int graphicsFamily = -1;			// Location of Graphics Queue family

	bool isValid() {
		return graphicsFamily >= 0;
	}
};