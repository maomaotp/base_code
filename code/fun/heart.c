#include <stdio.h>

int main()
{
	float y, x, a;
	for (y = 1.5f; y > -0.9f; y -= 0.1f) {
		for (x = -1.5f; x < 1.1f; x += 0.05f) {
			a = x * x + y * y - 1;
			putchar(a * a * a - x * x * y * y * y <= 0.0f ? '*' : ' ');
		}
		putchar('\n');
	}
}
