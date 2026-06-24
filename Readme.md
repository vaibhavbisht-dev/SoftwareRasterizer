# Software Rasterizer

> This is a Practical Implementation of a **Software Rasterizer** created by me in C++.

## What is a Software Rasterizer ?
A **Software Rasterizer** is a graphics engine that renders 3D objects into 2D images using CPU instead of a dedicated Graphics Hardware (**GPU**).

## Project Requirements
**This Project uses these following API and Languages:** 
- **C++**
- **SDL3 (For Rendering)**

## Creating a Frame Buffer
### What is a Frame Buffer?
Your monitor displays a picture Bu Lighting up a grid of pixels. Something has to decide what that each pixel should be. That something is a **FrameBuffer**. It is a block of memory where each slot corresponds to one pixel on the memory.<br>
Think of it as a flat 1D array representing a 2D grid.<br>
`std::vector<uint32_t> m_buffer;`
>Pixel at (x,y) lives at index: `y * width + x`<br>

But Why `y * width + x` and not `x * Height + y`?<br>

Let's Assume pixel in a screen is a grid: <br>

| | x=0 | x=1 | x=2 | x=3 |
| :--- | :---: | :---: | :---: | :---: |
| **y=0** | [A] | [B] | [C] | [D] |
| **y=1** | [E] | [F] | [G] | [H] |
| **y=2** | [I] | [J] | [K] | [L] |
| **y=3** | [M] | [N] | [O] | [P] |


And Memory is stored in a single Continuous line: <br>

And memory is stored in a continuous line.

| Memory Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | 
| **Stored Pixel** | [A] | [B] | [C] | [D] | [E] | [F] | [G] | [H] | [I]

So to find [F] pixel in memory: <br>
x = 1 <br>
y = 1 <br>
Width = 4

> Formula: `y * Width + x`. <br>

1 * 4 + 1 = 5<br>
That's How we Get the memory block location.

## Pixel Format
>As I am using SDL For Display. I'll use SDL_PIXELFORMAT_ARGB8888.

`SDL_PIXELFORMAT_ARGB8888` and the framebuffer will be `uint32_t` a 32 bit unsigned integer. Which means 1 pixel will take 32 bits = 4 bytes.<br>

The Window Resolution is `800 x 600` So that Means a total of `480000 pixels`.<br>
1 pixel = 4 bytes<br>
480000 pixels = 480000 x 4<br>
480000 pixels = 1920000 bytes = 1.92 MB<br>

**The format ARGB8888 means:**
- A  → Alpha → 8 bytes
- R  → Red → 8 bytes
- G  → Green → 8 bytes
- B  → Blue → 8 bytes

So, `0xFFFF0000` is fully opaque, full red with no green or blue.<br>

## Bresenham's Line Algorithm
>The Bresenham's line Algorithm is a highly efficient rasterization method used in computer graphics to approximate a mathematical straight line on a discreate pixel grid.

The algorithm is based on the equation of the line: y = mx + c. where., $$m = \frac{\Delta y}{\Delta x}$$
is the Slope.<br>

## Functions 
### Clear Screen
The most Basic Operation in any renderer is clearing the framebuffer before drawing anything.<br>
>I'll use `std::fill` as it is the most clear and idiomatic way.

```
void clearbuffer()
{
	std::fill(m_buffer.begin(), m_buffer.end(), 0xFF000000);
}
```
### Set Pixel
It is the most Basic function of the framebuffer as to draw anything You will have to Set a pixel to a given color.<br>
```
void setPixel(int x, int y, uint32_t color) {
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
	{
		m_buffer[y * m_width + x] = color;
	}
}
```
### Draw Line
Draw Line is one of the most important function of the framebuffer as the first step to draw anything is to create a line.
```
void DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if(x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	// as this is same for every line, we can calculate it once
	int dx = x1 - x0;
	int dy = y1 - y0;

  	// We use error term to decide when to step in y direction. The error term is the distance from the ideal line. When it exceeds a threshold (in this case, dx), we step in y direction and reset the error.
	int derror = std::abs(dy) * 2;
	int error = 0;


	int y = y0;

	for (int x = x0; x <= x1; x++) {
		if (steep) {
			setPixel(y, x, color); // if transposed, de−transpose 
		}
		else {
			setPixel(x, y, color);
		}

		// Update the error term and step in y direction if necessary
		error += derror;
		if (error > dx) {
			y += (y1 > y0 ? 1 : -1);
			error -= dx *2;
		}
	}
}
```

## Triangles
### Why Triangles?
>Every 3D Mesh (Characters, Objects, etc.) is stored as a list of triangles.

**Three Reasons:**

1. **Guaranteed Planarity:**  Three Points always define one flat plain. Four points might not. A Quad can bent. A Triangle can never. This makes lighting math exact.
2. **Guaranteed Convexity:** The Inside of a triangle is always convex shape. There are no caves or holes. This makes "Is this pixel Inside?" a Solvable problem with simple math.
3. **Hardware:** Every GPU in the world is build around the triangle as its atomic unit. Understanding Triangle Rasterization is understanding what GPU does.

### Filling the Inside of a Triangle

So, Now the Question is How do we fill every pixel inside a triangle?<br>
We have 2 main options:
- **Scan Lines**
- **Edge Function**

### Scan Lines
>for Each Horizontal rows of pixels the triangle touches, find where that triangles left right edges cross that row, then fill everything between them.

This works But it is Complicated to implement correctly(I did it some how and its working). You need to:
- Sort vertices by Y
- Handle the case where Horizontal edge exist
- Split the Triangle into a flat top and flat bottom part
- Track two Edge Slop simultaneously

**My Implementation:**
```
void DrawTriangle(Vector2<int> t0, Vector2<int> t1, Vector2<int> t2, uint32_t color)
{
	if (t0.y == t1.y && t0.y == t2.y) return;
	// Sort the vertices by y-coordinate ascending (t0.y <= t1.y <= t2.y)
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t1.y > t2.y) std::swap(t1, t2);
	if (t0.y > t1.y) std::swap(t0, t1);

	int total_height = t2.y - t0.y;
	if (total_height == 0) return; // Prevent division by zero

	// We will iterate through each horizontal line of the triangle, calculating the intersection points with the triangle edges and filling in the pixels between those points.
	for (int i = 0; i < total_height; i++)
	{
		// Determine if we are in the upper or lower part of the triangle
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		// Calculate the height of the current segment (upper or lower)
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		// Calculate the interpolation factors for the current y level
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
		// Interpolate the x-coordinates of the triangle edges
		int ax =				t0.x + (t2.x - t0.x) * alpha;
		int bx = second_half ? t1.x + (t2.x - t1.x) * beta : t0.x + (t1.x - t0.x) * beta;
		// Ensure ax is the leftmost point and bx is the rightmost point
		if (ax > bx) std::swap(ax, bx);
		// We are drawing a horizontal line from ax to bx at the current y level (t0.y + i)
		for (int x = ax; x <= bx; x++)
		{
			setPixel(x, t0.y + i, color);
		}
	}
}
```

### Edge Function

For Every Pixel, We run a test checking that the pixel is inside the triangle or not. If it is then we color it else we skip it. The Test is three simple Multiplications:
#### The 2D Cross Product
The two 2D Vector A and B. There cross product Gives a Scaler:<br>
`cross(A,B) = A.x * B.y - A.y * B.x `

The Sign of this will tell you:
- **Positive:** B is to the left of A
- **Negative:** B is to the right of A
- **Zero:** A and B are parallel (The point is Exactly on the edge)
  
### The Edge Function
For a Directed Edge From Point A To Point B and a test point P:

`E(P) = (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x)`

This is just The cross Product of the edge vector (B - A) and the vector from point `A` to point `P`.<br>
The Sign will tell us which side of the directed edge `P` is on. If we define our triangle constantly `(say, always Counterclockwise: v0 → v1, v1 → v2, v2 → v0)`, then the point is inside the triangle if and only if all three edge functions return a value >= 0.


#### Winding Order
The edge function you define the edges. Go Counterclockwise and inside is positive Go Clockwise and inside is negative.

**Counterclockwise (CCW)**
- All three edge function positive = inside
- This is what i am using

**Clockwise (CW)**
- All three edge function negative = inside
 
#### The Bounding Box Optimization
We can't loop over every pixel on an `600x800` screen for every triangle. that's `480,000` pixels every triangle, which is absurd.

Instead, We will compute **axis aligned bound box** of the three vertices:
```
min_x = min(x0, x1, x2)
max_x = max(x0, x1, x2)
min_y = min(y0, y1, y2)
max_y = max(y0, y1, y2)
```

Final:
```
void DrawTriangle(Vector2<int> t0, Vector2<int> t1, Vector2<int> t2, uint32_t color)
{
	
	int min_X = std::min({ t0.x, t1.x, t2.x });
	int max_X = std::max({ t0.x, t1.x, t2.x });
	int min_Y = std::min({ t0.y, t1.y, t2.y });
	int max_Y = std::max({ t0.y, t1.y, t2.y });

	min_X = std::max(0, min_X);
	max_X = std::min(m_width - 1, max_X);
	min_Y = std::max(0, min_Y);
	max_Y = std::min(m_height - 1, max_Y);

	for (int y = min_Y; y <= max_Y; y++) {
		for (int x = min_X; x <= max_X; x++) {
			// Check if the point (x, y) is inside the triangle
			// Implementation for point-in-triangle check would go here
			if (isPointInTriangle(Vector2<int>(x, y), t0, t1, t2)) {
				setPixel(x, y, color);
			}
		}
	}
}


bool isPointInTriangle(Vector2<int> p, Vector2<int> a, Vector2<int> b, Vector2<int> c) {
	
	int e0 = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
	int e1 = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
	int e2 = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x);

	return (e0 < 0 && e1 < 0 && e2 < 0);

}
```
**Test Result:**
![Screenshot-2026-06-24-191141.png](jb-image:img_1782308525117_94045a19e6eb78)

