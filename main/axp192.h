#ifndef MAIN_AXP192_H_
#define MAIN_AXP192_H_

void i2c_master_init(void);
uint8_t i2c_read(uint8_t reg);
void i2c_write(uint8_t reg, uint8_t data);
void AXP192_PowerOn(void);
void AXP192_ScreenBreath(uint8_t brightness);
void AXP192_EnableCoulombcounter(void);
void AXP192_DisableCoulombcounter(void);
void AXP192_StopCoulombcounter(void);
void AXP192_ClearCoulombcounter(void);

#endif /* MAIN_AXP192_H_ */

