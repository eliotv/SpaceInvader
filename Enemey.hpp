/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enemey.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: evanheum <evanheum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/14 16:00:36 by evanheum          #+#    #+#             */
/*   Updated: 2018/01/14 19:14:50 by jkrause          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENEMEY_HPP
# define ENEMEY_HPP

#include "Entity.hpp"
#include "Tracker.hpp"
#include <ncurses.h>
#include <string>

class Enemey: public virtual Entity {
	private:
		int			_locX;
		int			_locY;
		int			_ticks;
		int			_fireCurTick;
		int			_fireTick;
		std::string	_name;
	public:
		Enemey();
		Enemey(int maxY, int maxX, int y, int x);
		Enemey(Enemey const &src);
		Enemey	&operator=(Enemey const &rhs);
		~Enemey();

		void		onCollision(Entity &entity);
		void		tick(Tracker &tracker, WINDOW *win);
		int			getX(void);
		int			getY(void);
};

#endif
