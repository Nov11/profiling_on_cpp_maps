#pragma once

namespace db {
	class Noncopyable {
	protected:
		Noncopyable() {}
		~Noncopyable(){}
	private:
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable& operator=(const Noncopyable&) = delete;
	};

}