// BAnimation.cpp (this is -*- C++ -*-)
// 
// \author: Bjoern Giesler <bjoern@giesler.de>
// 
// 
// 
// $Author: giesler $
// $Locker$
// $Revision$
// $Date: 2002-08-19 10:41:28 +0200 (Mon, 19 Aug 2002) $

/* system includes */
/* (none) */

/* my includes */
#include "BAnimation.h"

BAnimation::BAnimation()
{
  _start = INT2FIX(0);
  _end = INT2FIX(100);
  _step = INT2FIX(1);
  _deleg = NULL;
  _running = false;
  _loop = NULL;
}

BAnimation::~BAnimation()
{
  if(_running) unschedule();
}

u8 BAnimation::FIX_SHIFT = 8;
BAnimation::FIXED BAnimation::FIX_SCALE = (1<<FIX_SHIFT);
float BAnimation::FIX_SCALEF = ((float)FIX_SCALE);
float BAnimation::FIX_SCALEF_ = ((1.0f)/FIX_SCALE);

void
BAnimation::addTarget(volatile uint32* target, FixedModifier modifier)
{
  AnimTarget tgt = { (volatile void*)target, false, modifier };
  _targets.push_back(tgt);
}

void
BAnimation::addTarget(volatile uint16* target, FixedModifier modifier)
{
  AnimTarget tgt = { (volatile void*)target, true, modifier };
  _targets.push_back(tgt);
}

void
BAnimation::clearTargets()
{
  _targets.clear();
}

void
BAnimation::setStartValue(int value)
{
  if(_running) return;
  _start = INT2FIX(value);
}

void
BAnimation::setStartValue(float value)
{
  if(_running) return;
  _start = FLOAT2FIX(value);
}

void
BAnimation::setStepValue(int value)
{
  if(_running) return;
  _step = INT2FIX(value);
}

void
BAnimation::setStepValue(float value)
{
  if(_running) return;
  _step = FLOAT2FIX(value);
}

void
BAnimation::setEndValue(int value)
{
  if(_running) return;
  _end = INT2FIX(value);
}

void
BAnimation::setEndValue(float value)
{
  if(_running) return;
  _end = FLOAT2FIX(value);
}

void
BAnimation::schedule(bool global)
{
  if(_running) return;
  
  _value = _start;
  BRunLoop* loop = _loop ? _loop : BRunLoop::currentRunLoop();
  if(global)
    BRunLoop::addGlobalVBlankReceiver(this);
  else
    loop->addVBlankReceiver(this);
  _global = global;
  if(_deleg) _deleg->animationDidStart(this);
  setTarget(_start);

  _running = true;
}

void
BAnimation::unschedule()
{
  if(!_running) return;

  BRunLoop* loop = _loop ? _loop : BRunLoop::currentRunLoop();

  if(_global)
    BRunLoop::removeGlobalVBlankReceiver(this);
  else
    loop->removeVBlankReceiver(this);
  if(_deleg) _deleg->animationDidFinish(this);
  if(_loop) _loop->stop();
  _running = false;
}

void
BAnimation::run()
{
  if(_running) return;

  if(_loop) delete _loop;
  _loop = new BRunLoop;
  schedule();
  _loop->run();
  delete _loop;
  _loop = NULL;
}
  
void
BAnimation::setDelegate(Delegate* deleg)
{
  _deleg = deleg;
}

void
BAnimation::setTarget(FIXED value)
{
  for(std::vector<AnimTarget>::iterator iter = _targets.begin();
      iter != _targets.end(); iter++)
    {
      FIXED val = value;
      if(iter->modifier) val = iter->modifier(val);
      if(iter->targetIs16Bit)
	{
	  volatile uint16* tgt = (volatile uint16*)iter->target;
	  *tgt = (uint16)val;
	}
      else
	{
	  volatile uint32* tgt = (volatile uint32*)iter->target;
	  *tgt = val;
	}
    }
}

void
BAnimation::runloopReachedVBlank(BRunLoop* loop)
{
  _value = FIX_ADD(_value, _step);
  if((_step > 0 && _value >= _end) ||
     (_step < 0 && _value <= _end)) 
    {
      _value = _end;
      unschedule();
    }
  else
    {
      if(_deleg)
	_deleg->animationDidChangeValue(this, _value);
      setTarget(_value);
    }
}

BAnimation::FIXED
BAnimation::pageflipModifierYDY(BAnimation::FIXED val)
{
  // val goes from 0 to 1
  if(val == 0) return 0;
  return BAnimation::FIX_DIV(BAnimation::INT2FIX(1), val);
}

BAnimation::FIXED
BAnimation::pageflipModifierCY(BAnimation::FIXED val)
{
  // val goes from 0 to 1
  BAnimation::FIXED ydy = pageflipModifierYDY(val);
  return BAnimation::FIX_MUL(BAnimation::INT2FIX(192),
			     BAnimation::FIX_SUB(BAnimation::INT2FIX(1), ydy));
}

#if BAnimation_test
int main(int argc, char **argv)
{
}
#endif /* BAnimation_test */
