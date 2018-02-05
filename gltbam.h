#ifndef __GLTBAM_H__
#define __GLTBAM_H__

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <GL/glew.h>

namespace gltbam
{
	typedef struct GLTList_t
	{
		bool isFree;
		short tSize;
		long tRIdx;

		float * tCoords;
		short tSampler;

		GLTList_t * prevFree;
		GLTList_t * prev;
		GLTList_t * next;
		GLTList_t * nextFree;
	} GLTList;

	typedef std::unordered_map<std::string, GLTList *> GLTMap;

	extern std::vector<GLuint *> textureIds;

	extern GLTList * listHead;
	extern GLTList * listIterator;
	extern GLTList * listTail;

	extern GLTMap tMap;
	extern int tMaxSize;
	const short MAX_TEXTURE_SPACE_COUNT = 16;

	void init(int maxSize);
	void init();
	void deinit();

	void print_list();
	void print_list_item(GLTList * item);

	std::string talloc(std::string textureFilename);
	void tfree(std::string textureTag);

	float * tcoords(std::string textureTag);
	short tsampler(std::string textureTag);
}

#endif
