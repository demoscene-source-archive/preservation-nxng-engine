//	------------------------------------------------------------------------------
//
//		nXv9 by xBaRr/Syndrome 2000
//			(nXs3) by xBaRr/Syndrome alias Emmanuel JULIEN
//			All rights reserved 2000.
//
//		Part of nXng.
//		mailto://xbarr@pingoo.com
//		http://xbarr.planet-d.net
//
//		nXng TexMap Linked List implementation file
//
//	------------------------------------------------------------------------------


	#include	"nX_HeaderList.h"
	#include	"IO/cqString.h"

	static		qString	cStr;
	static		char	str[256];


//--------------------------------------------------------------------
void	nX_TexMap::Release()
//--------------------------------------------------------------------
{

	RefCount--;
	if (!RefCount)
		delete this;

}


//--------------------------------------------------------------------
void	nX_TexMap::AddRef()
//--------------------------------------------------------------------
{
	RefCount++;
}


//--------------------------------------------------------------------
nX_TexMap	*nX::FindTexMap(char *id, nXTMC tcaps)
//			finds a TexMap in nX linked list using nXTMC criteria list
//--------------------------------------------------------------------
{

	nX_LLTexMap	*LLmap = LLtexmap;

	while (LLmap)
	{
		if (LLmap->nodemap && !cStr.qstrcmp(LLmap->nodemap->name, id) && (LLmap->nodemap->stdCAPS == tcaps))
			return LLmap->nodemap;
		LLmap = LLmap->next;
	}

	return NULL;

}


//--------------------------------------------------------------------
void	nX::RegisterTexMap(nX_TexMap *map)
//		register a TexMap in nX linked list
//--------------------------------------------------------------------
{

	nX_LLTexMap	*nLL = new nX_LLTexMap;
	nLL->prev = NULL;
	nLL->next = LLtexmap;
	if (LLtexmap)
		LLtexmap->prev = nLL;
	nLL->nodemap = map;
	LLtexmap = nLL;
	map->pnXLL = nLL;

}


//--------------------------------------------------------------------
void	nX::UnregisterTexMap(nX_LLTexMap *node)
//		unregister a TexMap from nX linked list
//--------------------------------------------------------------------
{

	if (node->prev && node->next)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	else if (!node->prev && node->next)
	{
		LLtexmap = node->next;
		LLtexmap->prev = NULL;
	}
	else if (!node->next && node->prev)
		node->prev->next = NULL;
	else if (!node->prev && !node->next)
		LLtexmap = NULL;

	delete	node;

}


//--------------------------------------------------------------------
void	nX_Scene::AddMapToLLTexMap(nX_TexMap *map)
//		unregister a TexMap from nX linked list
//--------------------------------------------------------------------
{

	nX_LLTexMap		*nLL = new nX_LLTexMap;
	nLL->next = NULL; nLL->prev = NULL;
	nLL->nodemap = map;

	if (LLtexmap)
	{
		nLL->next = LLtexmap;
		LLtexmap->prev = nLL;
	}
	LLtexmap = nLL;

}