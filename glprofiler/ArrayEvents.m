//
//  ArrayEvents.m
//  GLProfiler
//
//  Created by handley on 11/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ArrayEvents.h"

@implementation EnableClientStateEvent
- (uint32_t)array
{
	return data->array;
}
- (void)setArray:(uint32_t)array
{
	data->array = array;
}
- (NSString *)_name
{
	const char *arrayString;
	switch (self.array)
	{
	case GL_VERTEX_ARRAY:
		arrayString = "Vertex Array";
		break;
	case GL_COLOR_ARRAY:
		arrayString = "Color Array";
		break;
	default:
		arrayString = "Unknown";
		break;
	}
	return [NSString stringWithFormat:@"Enable Client State: %s",arrayString];
}
- (void)drawRect:(NSRect)rect
{
	glEnableClientState(self.array);
}
@end

@interface PointerEvent()
- (const char *)stringForType:(uint32_t)t;
@end

@implementation PointerEvent
- (uint32_t)size
{
	return data->size;
}
- (void)setSize:(uint32_t)size
{
	data->size = size;
}
- (uint32_t)type
{
	return data->type;
}
- (void)setType:(uint32_t)type
{
	data->type = type;
}
- (uint32_t)stride
{
	return data->stride;
}
- (void)setStride:(uint32_t)stride
{
	data->stride = stride;
}
- (uintptr_t)pointer
{
	return data->pointer;
}
- (void)setPointer:(uintptr_t)pointer
{
	data->pointer = pointer;
}
- (const char *)stringForType:(uint32_t)t
{
	switch (t)
	{
		case GL_BYTE:
			return "byte";
			break;
		case GL_UNSIGNED_BYTE:
			return "unsigned byte";
			break;
		case GL_SHORT:
			return "short";
			break;
		case GL_UNSIGNED_SHORT:
			return "unsigned short";
			break;
		case GL_INT:
			return "int";
			break;
		case GL_UNSIGNED_INT:
			return "unsigned int";
			break;
		case GL_FLOAT:
			return "float";
			break;
		case GL_DOUBLE:
			return "double";
			break;
		default:
			return "unknown";
			break;
	}
}
@end


@implementation VertexPointerEvent
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Vertex Pointer: (%u %s%s)", self.size, [self stringForType:self.type], self.size == 1 ? "" : "s"];
}
- (void)drawRect:(NSRect)rect
{
	glVertexPointer(self.size, self.type, self.stride, (void *)self.pointer);
}
@end
@implementation ColorPointerEvent
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Color Pointer: (%u %s%s)", self.size, [self stringForType:self.type], self.size == 1 ? "" : "s"];
}
- (void)drawRect:(NSRect)rect
{
	glColorPointer(self.size, self.type, self.stride, (void *)self.pointer);
}
@end
@implementation TexCoordPointerEvent
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Tex Coord Pointer: (%u %s%s)", self.size, [self stringForType:self.type], self.size == 1 ? "" : "s"];
}
- (void)drawRect:(NSRect)rect
{
	glTexCoordPointer(self.size, self.type, self.stride, (void *)self.pointer);
}
@end
@implementation NormalPointerEvent
- (NSString *)_name
{
	return [NSString stringWithFormat:@"Normal Pointer: (%ss)", [self stringForType:self.type]];
}
- (void)drawRect:(NSRect)rect
{
	glNormalPointer(self.type, self.stride, (void *)self.pointer);
}
@end
