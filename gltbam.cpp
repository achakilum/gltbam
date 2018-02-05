#include "gltbam.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

namespace gltbam
{
	std::vector<GLuint *> textureIds;

	GLTList * listHead = NULL;
	GLTList * listIterator = NULL;
	GLTList * listTail = NULL;

	GLTMap tMap;
	int tMaxSize = 0;

	void init(int textureMaxSize)
	{
		if (listHead && listTail)
		{
			std::cerr << "gltbam already initialized." << std::endl;
			return;
		}

		tMap.clear();
		textureIds.clear();
		tMaxSize = textureMaxSize;

		GLTList * tPrevSpace = NULL;
		for (int i = 0; i < MAX_TEXTURE_SPACE_COUNT; i++)
		{
			textureIds.push_back(NULL);

			GLTList * tNewSpace = new GLTList();
			tNewSpace->tSize = tMaxSize;
			tNewSpace->isFree = true;
			tNewSpace->tRIdx = i;
			tNewSpace->tCoords = NULL;
			tNewSpace->tSampler = i;

			if (i == 0)
			{
				listHead = tNewSpace;
				listIterator = tNewSpace;
			}

			listTail = tNewSpace;

			if (tPrevSpace)
			{
				tPrevSpace->next = tNewSpace;
				tNewSpace->prev = tPrevSpace;
			}

			tPrevSpace = tNewSpace;
		}
	}

	void init()
	{
		init(2048);
	}

	void deinit()
	{
		if (!listHead && !listTail)
		{
			std::cerr << "gltbam already deinitialized." << std::endl;
			return;
		}

		while (listHead)
		{
			GLTList * nodeToDelete = listHead;
			delete [] nodeToDelete->tCoords; nodeToDelete->tCoords = NULL;

			listHead = listHead->next;
			delete nodeToDelete; nodeToDelete = NULL;
		}

		for (int i = 0; i < MAX_TEXTURE_SPACE_COUNT; i++)
		{
			if (textureIds[i])
			{
				glDeleteTextures(1, textureIds[i]);
			}

			delete textureIds[i]; textureIds[i] = NULL;
		}

		listHead = NULL;
		listIterator = NULL;
		listTail = NULL;

		tMap.clear();
		tMaxSize = 0;
	}
	
	void print_list()
	{
		GLTList * curItem = listHead;
		
		std::cout << "GLTList (all items):" << std::endl;

		while (curItem)
		{
			print_list_item(curItem);
			curItem = curItem->next;
		}
		std::cout << std::endl;
	}

	void print_list_item(GLTList * item)
	{
		if (item)
		{
			std::cout << "(";
			std::cout << "isFree = " << ((item->isFree) ? "T" : "F") << ", ";
			std::cout << "tSize = " << item->tSize << ", ";
			std::cout << "tRIdx = " << item->tRIdx;
			std::cout << ")";
			std::cout << ((item == listHead) ? " head" : "");
			std::cout << ((item == listTail) ? " tail" : "");
			std::cout << ((item == listIterator) ? " iterator" : "");
			std::cout << std::endl;
		}
		else
		{
			std::cout << "(NULL)" << std::endl;
		}
	}

	std::string talloc(std::string textureFilename)
	{
		GLTList * listStop = listIterator;
		bool searchComplete = false;
		std::string textureTag = "";

		int w,h,n;
		unsigned char * image_data = stbi_load(textureFilename.c_str(), &w, &h, &n, STBI_rgb_alpha);
		int textureSize = std::max(w, h);

		if (!listIterator)
		{
			std::cerr << "iterator does not exist" << std::endl;
			stbi_image_free(image_data);
			return "";
		}

		if (textureSize > tMaxSize)
		{
			std::cerr << "no space for texture of size " << textureSize << std::endl;
			stbi_image_free(image_data);
			return "";
		}

		while (searchComplete == false)
		{
			if (textureSize > listIterator->tSize / 2 && textureSize <= listIterator->tSize && listIterator->isFree)
			{
				// Allocation
				GLTList * tToAllocate = listIterator;
				tToAllocate->isFree = false;
				textureTag = "" + std::to_string(tToAllocate->tSize) + "_" + std::to_string(tToAllocate->tRIdx) + "";
				tMap.insert({{textureTag, tToAllocate}});

				if (tToAllocate->tSize == tMaxSize)
				{
					textureIds[tToAllocate->tSampler] = new GLuint[1];

					glGenTextures(1, textureIds[tToAllocate->tSampler]);
					glActiveTexture(GL_TEXTURE0 + tToAllocate->tSampler);
					glBindTexture(GL_TEXTURE_2D, *textureIds[tToAllocate->tSampler]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tMaxSize, tMaxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *) NULL);
				}

				int idx = tToAllocate->tRIdx % ((tMaxSize / tToAllocate->tSize) * (tMaxSize / tToAllocate->tSize));

				int x = 0;
				int y = 0;

				int iter = 0;
				int addend = 1;

				while (idx > 0)
				{
				    int bit = idx & 1;

				    if (iter % 2 == 0)
				    {
					y += (bit == 1) ? addend : 0;
				    }
				    else
				    {
					x += (bit == 1) ? addend : 0;
					addend *= 2;
				    }

				    iter++;
				    idx = idx >> 1;
				}

				//"texCoords": [0.185546875, 0.001953125, 0.126953125, 0.001953125, 0.126953125, 0.05269680172204971, 0.185546875, 0.05269680172204971],
				//"texCoords": [hiX,		loY,		loX,	loY,		loX,		hiY,		hiX,		hiY		],

				int spacePlacementX = x * tToAllocate->tSize;
				int spacePlacementY = y * tToAllocate->tSize;

				float loX = spacePlacementX / (tMaxSize * 1.0f);
				float loY = spacePlacementY / (tMaxSize * 1.0f);
				float hiX = (spacePlacementX + w) / (tMaxSize * 1.0f);
				float hiY = (spacePlacementY + h) / (tMaxSize * 1.0f);

				float * tCoords = new float[8];
				tCoords[0] = hiX; tCoords[1] = loY;
				tCoords[2] = loX; tCoords[3] = loY;
				tCoords[4] = loX; tCoords[5] = hiY;
				tCoords[6] = hiX; tCoords[7] = hiY;
				
				tToAllocate->tCoords = tCoords;

				glActiveTexture(GL_TEXTURE0 + tToAllocate->tSampler);
				glBindTexture(GL_TEXTURE_2D, *textureIds[tToAllocate->tSampler]);
				glTexSubImage2D(GL_TEXTURE_2D, 0, spacePlacementX, spacePlacementY, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
				
				std::cerr << "(+) texture allocated with tag " << textureTag << std::endl << std::endl;
				searchComplete = true;
			}
			else
			if (textureSize <= listIterator->tSize / 2 && listIterator->isFree)
			{	
				GLTList * tToSplit = listIterator;

				if (tToSplit->tSize == tMaxSize)
				{
					textureIds[tToSplit->tSampler] = new GLuint[1];

					glGenTextures(1, textureIds[tToSplit->tSampler]);
					glActiveTexture(GL_TEXTURE0 + tToSplit->tSampler);
					glBindTexture(GL_TEXTURE_2D, *textureIds[tToSplit->tSampler]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tMaxSize, tMaxSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *) NULL);
				}

				// Grow list with new sublist
				GLTList * next1 = new GLTList();
				GLTList * next2 = new GLTList();
				GLTList * next3 = new GLTList();

				next1->isFree = true;
				next2->isFree = true;
				next3->isFree = true;

				short tNewSize = tToSplit->tSize >> 1;
				tToSplit->tSize = tNewSize;
				next1->tSize = tNewSize;
				next2->tSize = tNewSize;
				next3->tSize = tNewSize;

				int tNewIdxBase = tToSplit->tRIdx << 2;
				tToSplit->tRIdx = tNewIdxBase;
				next1->tRIdx = tNewIdxBase + 1;
				next2->tRIdx = tNewIdxBase + 2;
				next3->tRIdx = tNewIdxBase + 3;

				next1->tCoords = NULL;
				next2->tCoords = NULL;
				next3->tCoords = NULL;

				next1->tSampler = tToSplit->tSampler;
				next2->tSampler = tToSplit->tSampler;
				next3->tSampler = tToSplit->tSampler;

				// Whole sublist linking
				GLTList * curItemNext = tToSplit->next;

				tToSplit->next = next1;
				next1->next = next2;
				next2->next = next3;
				next3->next = curItemNext;

				if (curItemNext)
				{
					curItemNext->prev = next3;
				}

				next3->prev = next2;
				next2->prev = next1;
				next1->prev = tToSplit;

				if (listIterator == listTail)
				{
					listTail = next3;
				}

				continue;
			}

			// Loop around list or iterate to next list item
			if (listIterator == listTail)
			{
				listIterator = listHead;
			}
			else
			{
				listIterator = listIterator->next;
			}

			if (listIterator == listStop)
			{	
				searchComplete = true;
			}
		} 

		if (textureTag.length() <= 0)
		{
			std::cerr << "no space found for texture of size " << textureSize << std::endl;
		}

		stbi_image_free(image_data);
		return textureTag;
	}

	void tfree(std::string textureTag)
	{
		auto tFreeIterator = tMap.find(textureTag);

		if (tFreeIterator != tMap.end())
		{
			GLTList * tFree = tFreeIterator->second;
			tFree->isFree = true;
			delete [] tFree->tCoords; tFree->tCoords = NULL;

			tMap.erase(textureTag);
			std::cerr << "(-) texture with tag " << textureTag << " found and freed" << std::endl;

			// Free list coalescing
			while (tFree->tSize < tMaxSize)
			{
				bool coalesceFailure = false;

				// Search for blocks to coalesce
				std::vector<GLTList *> tToCoalesce(4, NULL);
				int tFreeIdx = tFree->tRIdx & 3;
				tToCoalesce[tFreeIdx] = tFree;

				GLTList * tFreeRight = tFree->next;
				for (int i = tFreeIdx + 1; i <= 3; i++)
				{
					if (tFreeRight && // next node exists
					    tFreeRight->isFree && // next node is free
					   (tFreeRight->tRIdx & (~3)) == (tFree->tRIdx & (~3)) && // next node and tFree belong in the same group
					   (tFreeRight->tRIdx & 3) == i && // next node group index matches expected index
					    tFreeRight->tSize == tFree->tSize) // next node size matches expected size
					{
						tToCoalesce[i] = tFreeRight;
						tFreeRight = tFreeRight->next;
					}	
					else
					{
						coalesceFailure = true;
					}
				}

				GLTList * tFreeLeft = tFree->prev;
				for (int i = tFreeIdx - 1; i >= 0; i--)
				{
					if (tFreeLeft && // next node exists
					    tFreeLeft->isFree && // next node is free
					   (tFreeLeft->tRIdx & (~3)) == (tFree->tRIdx & (~3)) && // next node and tFree belong in the same group
					   (tFreeLeft->tRIdx & 3) == i && // next node group index matches expected index
					    tFreeLeft->tSize == tFree->tSize) // next node size matches expected size
					{
						tToCoalesce[i] = tFreeLeft;
						tFreeLeft = tFreeLeft->prev;
					}
					else
					{
						coalesceFailure = true;
					}
				}

				if (coalesceFailure)
				{
					break;
				}
				
				std::cout << "[-] Coalescing blocks..." << std::endl;
				for (int i = 0; i < tToCoalesce.size(); i++)
				{
					print_list_item(tToCoalesce[i]);
					if (tToCoalesce[i] == listHead)
					{
						listHead = tToCoalesce[0];
					}

					if (tToCoalesce[i] == listTail)
					{
						listTail = tToCoalesce[0];
					}

					if (tToCoalesce[i] == listIterator)
					{
						listIterator = tToCoalesce[0];
					}
				}

				// Coalesce blocks that were found
				tToCoalesce[0]->next = tToCoalesce[3]->next;
				if (tToCoalesce[3]->next)
				{
					tToCoalesce[3]->next->prev = tToCoalesce[0];
				}

				tToCoalesce[0]->tSize = tToCoalesce[0]->tSize << 1;
				tToCoalesce[0]->tRIdx = tToCoalesce[0]->tRIdx >> 2;

				delete tToCoalesce[1]; tToCoalesce[1] = NULL;
				delete tToCoalesce[2]; tToCoalesce[2] = NULL;
				delete tToCoalesce[3]; tToCoalesce[3] = NULL;

				tFree = tToCoalesce[0];

				std::cout << std::endl;
			}

			int w,h,n;
			unsigned char * empty = new unsigned char[tFree->tSize * tFree->tSize * 4];
			std::memset(empty, 0, tFree->tSize * tFree->tSize * 4 * sizeof(unsigned char));

			int idx = tFree->tRIdx % ((tMaxSize / tFree->tSize) * (tMaxSize / tFree->tSize));

			int x = 0;
			int y = 0;

			int iter = 0;
			int addend = 1;

			while (idx > 0)
			{
			    int bit = idx & 1;

			    if (iter % 2 == 0)
			    {
				y += (bit == 1) ? addend : 0;
			    }
			    else
			    {
				x += (bit == 1) ? addend : 0;
				addend *= 2;
			    }

			    iter++;
			    idx = idx >> 1;
			}

			int spacePlacementX = x * tFree->tSize;
			int spacePlacementY = y * tFree->tSize;

			glActiveTexture(GL_TEXTURE0 + tFree->tSampler);
			glBindTexture(GL_TEXTURE_2D, *textureIds[tFree->tSampler]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, spacePlacementX, spacePlacementY, tFree->tSize, tFree->tSize, GL_RGBA, GL_UNSIGNED_BYTE, empty);

			stbi_image_free(empty);
	
			if (tFree->tSize == tMaxSize)
			{
				glDeleteTextures(1, textureIds[tFree->tSampler]);
				delete textureIds[tFree->tSampler]; textureIds[tFree->tSampler] = NULL;
			}
		}
	}

	float * tcoords(std::string textureTag)
	{
		auto tFreeIterator = tMap.find(textureTag);
		float * coords = NULL;

		if (tFreeIterator != tMap.end())
		{
			GLTList * tFound = tFreeIterator->second;
			coords = tFound->tCoords;
		}

		return coords;
	}

	short tsampler(std::string textureTag)
	{
		auto tFreeIterator = tMap.find(textureTag);
		short sampler = -1;

		if (tFreeIterator != tMap.end())
		{
			GLTList * tFound = tFreeIterator->second;
			sampler = tFound->tSampler;
		}

		return sampler;
	}
}
