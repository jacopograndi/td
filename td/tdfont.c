#include "tdfont.h";

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "linmath.h"

void tdfont_init(Tdf* tdf) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		printf("ERROR::FREETYPE: Could not init FreeType Library\n");
	}
	FT_Face face;
	if (FT_New_Face(ft,"fonts/OpenSans-Light.ttf",0,&face)) {
		printf("ERROR::FREETYPE: Failed to load font\n");
	}
	FT_Set_Pixel_Sizes(face,0,48);
	if (FT_Load_Char(face,'X',FT_LOAD_RENDER)) {
		printf("ERROR::FREETYTPE: Failed to load Glyph\n");
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1); // Disable byte-alignment restriction

	for (GLubyte c=0; c<128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face,c,FT_LOAD_RENDER))
		{
			printf("ERROR::FREETYTPE: Failed to load Glyph\n");
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		// Now store character for later use
		tdf->fchars[c].TextureID=texture;
		tdf->fchars[c].Advance=face->glyph->advance.x;
		tdf->fchars[c].Size[0]=face->glyph->bitmap.width;
		tdf->fchars[c].Size[1]=face->glyph->bitmap.rows;
		tdf->fchars[c].Bearing[0]=face->glyph->bitmap_left;
		tdf->fchars[c].Bearing[1]=face->glyph->bitmap_top;
	}

	glGenVertexArrays(1,&tdf->VAO);
	glGenBuffers(1,&tdf->VBO);
	glBindVertexArray(tdf->VAO);
	glBindBuffer(GL_ARRAY_BUFFER,tdf->VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*6*4,NULL,GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}

void tdfont_render_text(Tdf* tdf, int shader,char* text,GLfloat x,GLfloat y,GLfloat scale,
	vec4 color,vec2 res)
{
	// Activate corresponding render state	
	glUseProgram(shader);
	mat4x4 font_proj;
	mat4x4_ortho(font_proj,0.0f,res[0],0.0f,res[1],0.0f,1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,GL_FALSE,(const GLfloat*)font_proj);
	glUniform4f(glGetUniformLocation(shader,"textColor"),color[0],color[1],color[2],color[3]);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(tdf->VAO);

	// Iterate through all characters
	for (int i=0; text[i]!='\0'; i++) {
		Fcharacter* ch=&(tdf->fchars[text[i]]);
		GLfloat xpos=x+ch->Bearing[0]*scale;
		GLfloat ypos=y-(ch->Size[1]-ch->Bearing[1])*scale;

		GLfloat w=ch->Size[0]*scale;
		GLfloat h=ch->Size[1]*scale;
		// Update VBO for each character

		GLfloat vertices[6][4]={
			{xpos,ypos+h,0.0,0.0},
			{xpos,ypos,0.0,1.0},
			{xpos+w,ypos,1.0,1.0},

			{xpos,ypos+h,0.0,0.0},
			{xpos+w,ypos,1.0,1.0},
			{xpos+w,ypos+h,1.0,0.0}
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D,ch->TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER,tdf->VBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices),vertices);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		// Render quad
		glDrawArrays(GL_TRIANGLES,0,6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x+=(ch->Advance>>6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D,0);
}