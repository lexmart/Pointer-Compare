#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "clang-c/Index.h"

#define assert(expr) if(!(expr)) { *((int *)0) = 0; }

enum Depth_State
{
	UNVISITED = 0,
	VISITED_FIRST_OP_AND_IS_NOT_ATOM,
	VISITED_FIRST_OP_AND_IS_ATOM,
};

static CXTypeKind atom_type = CXTypeKind::CXType_NullPtr;

#define MAX_AST_DEPTH (1024*1024)
static uint8_t depth_states[MAX_AST_DEPTH]; // 0 = haven't visited first operand yet, 1 = visited first operand 

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, void *depth_ptr)
{
	uint64_t ast_depth = (uint64_t)depth_ptr;
	assert(ast_depth < MAX_AST_DEPTH);

	CXCursorKind cursor_kind = clang_getCursorKind(cursor);
	CXCursorKind parent_kind = clang_getCursorKind(parent);
	CXType parent_type = clang_getCursorType(parent);

	if (parent_kind == CXCursorKind::CXCursor_BinaryOperator && parent_type.kind == CXTypeKind::CXType_Bool)
	{
		CXType type = clang_getCursorType(cursor);
		bool is_pointer = (type.kind == CXTypeKind::CXType_Pointer) || (type.kind == CXTypeKind::CXType_MemberPointer) || (type.kind == CXType_BlockPointer);
		if (atom_type == CXTypeKind::CXType_NullPtr && is_pointer)
		{
			CXType pointee_type = clang_getPointeeType(type);
			const char *spelling = (const char *)clang_getTypeSpelling(pointee_type).data;
			if (!strcmp(spelling, "Atom"))
			{
				atom_type = pointee_type.kind;
			}
		}

		int is_atom = is_pointer && (clang_getPointeeType(type).kind == atom_type);
		if(depth_states[ast_depth] == UNVISITED)
		{
			if (is_atom)
			{
				depth_states[ast_depth] = VISITED_FIRST_OP_AND_IS_ATOM;
			}
			else
			{
				depth_states[ast_depth] = VISITED_FIRST_OP_AND_IS_NOT_ATOM;
			}
		}
		else
		{
			if (depth_states[ast_depth] == VISITED_FIRST_OP_AND_IS_ATOM && is_atom)
			{
				CXSourceLocation location = clang_getCursorLocation(parent);
				CXFile file;
				unsigned int line, col, offset;
				clang_getSpellingLocation(location, &file, &line, &col, &offset);
				CXString file_name = clang_getFileName(file);
				printf("Comparing atom pointers in %s on line %u\n", file_name.data, line);
			}

			depth_states[ast_depth] = UNVISITED;
		}
	}
	
	clang_visitChildren(cursor, visitor, (void *)(ast_depth + 1));

	return CXChildVisit_Continue;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: analyze main.cpp");
		exit(0);
	}

	memset(depth_states, 0, sizeof(depth_states));

	CXIndex index = clang_createIndex(0, 0);
  	CXTranslationUnit unit = clang_parseTranslationUnit(index, argv[1], 0, 0, 0, 0, CXTranslationUnit_None);
	if (unit)
	{
		CXCursor cursor = clang_getTranslationUnitCursor(unit);
		clang_visitChildren(cursor, visitor, 0);

		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);

#if false
		if (!state.compared_two_atoms)
		{
			printf("No atom comparisons detected\n");
		}
#endif
	}
	else
	{
		printf("Failed to parse %s\n", argv[1]);
	}
}