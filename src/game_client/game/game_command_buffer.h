

#ifndef GAME_COMMAND_BUFFER_H
#define GAME_COMMAND_BUFFER_H


class CCommandBuffer
{
public:
	bool	Push(int);
	int		Pop();
	void	Clear();

private:
};



#endif//GAME_COMMAND_BUFFER_H
