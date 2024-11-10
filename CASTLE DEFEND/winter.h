#pragma once

#ifdef WINTER_EXPORTS
#define WINTER_API __declspec(dllexport)
#else
#define WINTER_API __declspec(dllimport)
#endif

#include <random>
#include <vector>

constexpr char up_flag{ 0b00000001 };
constexpr char down_flag{ 0b00000010 };
constexpr char left_flag{ 0b00000100 };
constexpr char right_flag{ 0b00001000 };

constexpr char evil1_flag{ 0b00000001 };
constexpr char evil2_flag{ 0b00000010 };
constexpr char evil3_flag{ 0b00000100 };
constexpr char evil4_flag{ 0b00001000 };
constexpr char evil5_flag{ 0b00010000 };

constexpr char turret1_flag{ 0b00000001 };
constexpr char turret2_flag{ 0b00000010 };
constexpr char turret3_flag{ 0b00000100 };
constexpr char turret4_flag{ 0b00001000 };
constexpr char turret5_flag{ 0b00010000 };
constexpr char turret6_flag{ 0b00100000 };

constexpr float scr_width(800.0f);
constexpr float scr_height(700.0f);
constexpr float sky(50.0f);
constexpr float ground(650.0f);

struct GAME_COORD
{
	float x{ 0 };
	float y{ 0 };
};

namespace game
{
	class WINTER_API RANDERER
	{
		private:
			std::random_device rd{};
			std::seed_seq* sequencer = nullptr;
			std::vector<unsigned int> vSeeds;
			std::mt19937* twister = nullptr;
	
		public:
			RANDERER();
			~RANDERER();

			int operator() (int min, int max);
	};

	class WINTER_API SIMPLE
	{
		protected:
			float width{ 1.0f };
			float height{ 1.0f };

		public:
			float x{ 0 };
			float y{ 0 };
			float ex{ 0 };
			float ey{ 0 };

			SIMPLE(float _x = 0, float _y = 0, float _width = 1.0f, float _height = 1.0f);
			virtual ~SIMPLE() {};

			float GetWidth() const;
			float GetHeight() const;
			void SetWidth(float new_width);
			void SetHeight(float new_height);

			void NewDims(float new_width, float new_height);
			void SetEdges();
	};

	class WINTER_API SIMPLE_PACK
	{
		private:
			SIMPLE* base_ptr{ nullptr };
			size_t lenght{ 0 };
			size_t next_position{ 0 };
			bool state_of_pack = false;

		public:

			SIMPLE_PACK(size_t size) :lenght{ size }, base_ptr{ new SIMPLE[size] }
			{
				if (size > 0)state_of_pack = true;
			}
			~SIMPLE_PACK();

			bool IsValid() const;
			size_t FreeCapacity() const;

			size_t size() const;
			void push_back(SIMPLE& element);
			void push_front(SIMPLE& element);
			SIMPLE& operator[](size_t position);
			void operator()(SIMPLE& element, size_t position);
	};

	class WINTER_API CREATURE :public SIMPLE
	{
		protected:

			unsigned char flags{ 0b00000000 };
			unsigned char move_flags{ 0b00000000 };

			int max_frames{ 0 };
			int frame_delay{ 0 };
			int current_frame{ 0 };

			float speed{ 0 };

			float move_x{ 0 };
			float move_y{ 0 };
			float move_ex{ 0 };
			float move_ey{ 0 };
			float slope{ 0 };
			float intercept{ 0 };

			int strenght{ 0 };
			int attack_delay{ 0 };

			bool hor_path = false;
			bool vert_path = false;
			
		public:

			CREATURE(float _sx, float _sy);
			virtual ~CREATURE() {};

			void SetPathInfo(float _endx, float _endy);
			int GetDistance(GAME_COORD first_point, GAME_COORD second_point);
			bool GetFlag(unsigned char which_flag) const;
			void SetFlag(unsigned char which_flag);
			void NullFlag(unsigned char which_flag);
			GAME_COORD SortPack(SIMPLE_PACK& Package);


			virtual unsigned char Move(float gear, SIMPLE_PACK& enemies) = 0;
			virtual void Release() = 0;
			virtual int Attack() = 0;
			virtual int GetFrame() = 0;
			virtual void Transform(unsigned char to_what) = 0;
	};

	class WINTER_API EVILS :public CREATURE
	{
		protected:

			EVILS(unsigned char _type, float _sx, float _sy);

		public:
			int lifes{ 0 };

			friend EVILS* EvilFactory(unsigned char _type, float _sx, float _sy);

			unsigned char Move(float gear, SIMPLE_PACK& enemies) override;
			void Release() override;
			int Attack() override;
			int GetFrame() override;
			void Transform(unsigned char to_what) override;
	};

	class WINTER_API HEROES :public CREATURE
	{
		protected:
			HEROES(unsigned char _what_type, float _sx, float _sy);

		public:
			int lifes{ 0 };

			friend HEROES* TurretFactory(unsigned char _what_type, float _sx, float _sy);

			unsigned char Move(float gear, SIMPLE_PACK& enemies) override;
			void Release() override;
			int Attack() override;
			int GetFrame() override;
			void Transform(unsigned char to_what) override;
	};

	typedef EVILS* evil_ptr;
	typedef HEROES* turret_ptr;
}