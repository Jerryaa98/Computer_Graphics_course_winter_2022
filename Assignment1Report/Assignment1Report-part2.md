# Assignment1Report_part2

Using std we added a printing function to the code which prints the vectors of the vertices and faces, like in the image below :-

object used in the demo: bunny

```c++
if (lineType == "v")
		{
     vertices.push_back(Utils::Vec3fFromStream(issLine));
		 std::cout << issLine.str() << std::endl;
		}
		else if (lineType == "vn")
		{
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			// TODO: Handle texture coordinates
		}
		else if (lineType == "f")
		{
			faces.push_back(Face(issLine));
			std::cout << issLine.str() << std::endl;

		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			// std::cout << "Found unknown line Type \"" << lineType << "\"";
		}
```
