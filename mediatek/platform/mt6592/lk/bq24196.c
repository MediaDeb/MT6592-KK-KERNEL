#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_i2c.h>   
#include <platform/mt_pmic.h>
#include <platform/bq24196.h>
#include <printf.h>

//#include <target/cust_charging.h>

int g_bq24196_log_en=0;

/**********************************************************
  *
  *   [I2C Slave Setting] 
  *
  *********************************************************/
#define bq24196_SLAVE_ADDR_WRITE   0xD6
#define bq24196_SLAVE_ADDR_READ    0xD7

/**********************************************************
  *
  *   [Global Variable] 
  *
  *********************************************************/
#define bq24196_REG_NUM 11
kal_uint8 bq24196_reg[bq24196_REG_NUM] = {0};

/**********************************************************
  *
  *   [I2C Function For Read/Write bq24196] 
  *
  *********************************************************/
#if 0
#ifndef BQ24196_BUSNUM
#define BQ24196_BUSNUM I2C2
#endif  

U32 bq24196_i2c_read (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen)
{
    U32 ret_code = I2C_OK;

    ret_code = mt_i2c_write(BQ24196_BUSNUM, chip, cmdBuffer, cmdBufferLen, 1);	 // set register command
    if (ret_code != I2C_OK)
    return ret_code;

    ret_code = mt_i2c_read(BQ24196_BUSNUM, chip, dataBuffer, dataBufferLen, 1);

    //printf("[bq24196_i2c_read] Done\n");

    return ret_code;
}
	
U32 bq24196_i2c_write (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen)
{
    U32 ret_code = I2C_OK;
    U8 write_data[I2C_FIFO_SIZE];
    int transfer_len = cmdBufferLen + dataBufferLen;
    int i=0, cmdIndex=0, dataIndex=0;
    	
    if(I2C_FIFO_SIZE < (cmdBufferLen + dataBufferLen))
    {
        printf("[bq24196_i2c_write] exceed I2C FIFO length!! \n");
        return 0;
    }
    	
    //write_data[0] = cmd;
    //write_data[1] = writeData;
    
    while(cmdIndex < cmdBufferLen)
    {
        write_data[i] = cmdBuffer[cmdIndex];
        cmdIndex++;
        i++;
    }
    
    while(dataIndex < dataBufferLen)
    {
        write_data[i] = dataBuffer[dataIndex];
        dataIndex++;
        i++;
    }
    	
    /* dump write_data for check */
    for( i=0 ; i < transfer_len ; i++ )
    {
        //printf("[bq24196_i2c_write] write_data[%d]=%x\n", i, write_data[i]);
    }
    	
    ret_code = mt_i2c_write(BQ24196_BUSNUM, chip, write_data, transfer_len, 1);
    	
    //printf("[bq24196_i2c_write] Done\n");
	
    return ret_code;
}
#else
#define BQ24196_I2C_ID	I2C2
static struct mt_i2c_t bq24196_i2c;

kal_uint32 bq24196_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    bq24196_i2c.id = BQ24196_I2C_ID;
    /* Since i2c will left shift 1 bit, we need to set BQ24196 I2C address to >>1 */
    bq24196_i2c.addr = (bq24196_SLAVE_ADDR_WRITE >> 1);
    bq24196_i2c.mode = ST_MODE;
    bq24196_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&bq24196_i2c, write_data, len);
    //dprintf(INFO, "%s: i2c_write: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}

kal_uint32 bq24196_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer) 
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint16 len;
    *dataBuffer = addr;

    bq24196_i2c.id = BQ24196_I2C_ID;
    /* Since i2c will left shift 1 bit, we need to set BQ24196 I2C address to >>1 */
    bq24196_i2c.addr = (bq24196_SLAVE_ADDR_READ >> 1);
    bq24196_i2c.mode = ST_MODE;
    bq24196_i2c.speed = 100;
    len = 1;

    ret_code = i2c_write_read(&bq24196_i2c, dataBuffer, len, len);
    //dprintf(INFO, "%s: i2c_read: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}
#endif
/**********************************************************
  *
  *   [Read / Write Function] 
  *
  *********************************************************/
kal_uint32 bq24196_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
#if 0
    U8 chip_slave_address = bq24196_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;
    U32 result_tmp;

    //printf("--------------------------------------------------\n");

    cmd = RegNum;
    result_tmp = bq24196_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

    //printf("[bq24196_read_interface] Reg[%x]=0x%x\n", RegNum, data);
    
    data &= (MASK << SHIFT);
    *val = (data >> SHIFT);
    
    //printf("[bq24196_read_interface] val=0x%x\n", *val);

    if(g_bq24196_log_en>1)        
        printf("%d\n", result_tmp);

    return 1;
#else
    kal_uint8 bq24196_reg = 0;
    int ret = 0;

    dprintf(INFO, "--------------------------------------------------LK\n");

    ret = bq24196_read_byte(RegNum, &bq24196_reg);
    dprintf(INFO, "[bq24196_read_interface] Reg[%x]=0x%x\n", RegNum, bq24196_reg);

    bq24196_reg &= (MASK << SHIFT);
    *val = (bq24196_reg >> SHIFT);	  

    if(g_bq24196_log_en>1)		  
	    dprintf(INFO, "%d\n", ret);

    return ret;
#endif
}

kal_uint32 bq24196_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
#if 0
    U8 chip_slave_address = bq24196_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;
    U32 result_tmp;

    //printf("--------------------------------------------------\n");

    cmd = RegNum;
    result_tmp = bq24196_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    //printf("[bq24196_config_interface] Reg[%x]=0x%x\n", RegNum, data);

    data &= ~(MASK << SHIFT);
    data |= (val << SHIFT);

    result_tmp = bq24196_i2c_write(chip_slave_address, &cmd, cmd_len, &data, data_len);
    //printf("[bq24196_config_interface] write Reg[%x]=0x%x\n", RegNum, data);

    // Check
    result_tmp = bq24196_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    //printf("[bq24196_config_interface] Check Reg[%x]=0x%x\n", RegNum, data);

    if(g_bq24196_log_en>1)        
        printf("%d\n", result_tmp);
    
    return 1;
#else
    kal_uint8 bq24196_reg = 0;
    kal_uint32 ret = 0;

    dprintf(INFO, "--------------------------------------------------LK\n");

    ret = bq24196_read_byte(RegNum, &bq24196_reg);

    dprintf(INFO, "[bq24196_config_interface] Reg[%x]=0x%x\n", RegNum, bq24196_reg);

    bq24196_reg &= ~(MASK << SHIFT);
    bq24196_reg |= (val << SHIFT);

    ret = bq24196_write_byte(RegNum, bq24196_reg);

    dprintf(INFO, "[bq24196_config_interface] write Reg[%x]=0x%x\n", RegNum, bq24196_reg);

    // Check
    //bq24196_read_byte(RegNum, &bq24196_reg);
    //dprintf(INFO, "[bq24196_config_interface] Check Reg[%x]=0x%x\n", RegNum, bq24196_reg);

    if(g_bq24196_log_en>1)        
        dprintf(INFO, "%d\n", ret);

    return ret;
#endif
}

/**********************************************************
  *
  *   [Internal Function] 
  *
  *********************************************************/
//CON0----------------------------------------------------

void bq24196_set_en_hiz(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON0), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON0_EN_HIZ_MASK),
                                    (kal_uint8)(CON0_EN_HIZ_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);	
}

void bq24196_set_vindpm(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON0), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON0_VINDPM_MASK),
                                    (kal_uint8)(CON0_VINDPM_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_iinlim(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON0), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON0_IINLIM_MASK),
                                    (kal_uint8)(CON0_IINLIM_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON1----------------------------------------------------

void bq24196_set_reg_rst(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON1), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_REG_RST_MASK),
                                    (kal_uint8)(CON1_REG_RST_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_wdt_rst(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON1), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_WDT_RST_MASK),
                                    (kal_uint8)(CON1_WDT_RST_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_chg_config(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON1), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_CHG_CONFIG_MASK),
                                    (kal_uint8)(CON1_CHG_CONFIG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_sys_min(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON1), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_SYS_MIN_MASK),
                                    (kal_uint8)(CON1_SYS_MIN_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_boost_lim(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON1), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON1_BOOST_LIM_MASK),
                                    (kal_uint8)(CON1_BOOST_LIM_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON2----------------------------------------------------

void bq24196_set_ichg(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON2), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON2_ICHG_MASK),
                                    (kal_uint8)(CON2_ICHG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON3----------------------------------------------------

void bq24196_set_iprechg(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON3), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON3_IPRECHG_MASK),
                                    (kal_uint8)(CON3_IPRECHG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_iterm(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON3), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON3_ITERM_MASK),
                                    (kal_uint8)(CON3_ITERM_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON4----------------------------------------------------

void bq24196_set_vreg(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON4), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON4_VREG_MASK),
                                    (kal_uint8)(CON4_VREG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_batlowv(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON4), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON4_BATLOWV_MASK),
                                    (kal_uint8)(CON4_BATLOWV_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_vrechg(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON4), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON4_VRECHG_MASK),
                                    (kal_uint8)(CON4_VRECHG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON5----------------------------------------------------

void bq24196_set_en_term(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON5), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON5_EN_TERM_MASK),
                                    (kal_uint8)(CON5_EN_TERM_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_term_stat(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON5), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON5_TERM_STAT_MASK),
                                    (kal_uint8)(CON5_TERM_STAT_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_watchdog(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON5), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON5_WATCHDOG_MASK),
                                    (kal_uint8)(CON5_WATCHDOG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_en_timer(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON5), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON5_EN_TIMER_MASK),
                                    (kal_uint8)(CON5_EN_TIMER_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_chg_timer(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON5), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON5_CHG_TIMER_MASK),
                                    (kal_uint8)(CON5_CHG_TIMER_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON6----------------------------------------------------

void bq24196_set_treg(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON6), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON6_TREG_MASK),
                                    (kal_uint8)(CON6_TREG_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON7----------------------------------------------------

void bq24196_set_tmr2x_en(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON7), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON7_TMR2X_EN_MASK),
                                    (kal_uint8)(CON7_TMR2X_EN_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_batfet_disable(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON7), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON7_BATFET_Disable_MASK),
                                    (kal_uint8)(CON7_BATFET_Disable_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

void bq24196_set_int_mask(kal_uint32 val)
{
    kal_uint32 ret=0;    

    ret=bq24196_config_interface(   (kal_uint8)(bq24196_CON7), 
                                    (kal_uint8)(val),
                                    (kal_uint8)(CON7_INT_MASK_MASK),
                                    (kal_uint8)(CON7_INT_MASK_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);		
}

//CON8----------------------------------------------------

kal_uint32 bq24196_get_system_status(void)
{
    kal_uint32 ret=0;
    kal_uint8 val=0;

    ret=bq24196_read_interface(     (kal_uint8)(bq24196_CON8), 
                                    (&val),
                                    (kal_uint8)(0xFF),
                                    (kal_uint8)(0x0)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);	
	
    return val;
}

kal_uint32 bq24196_get_vbus_stat(void)
{
    kal_uint32 ret=0;
    kal_uint8 val=0;

    ret=bq24196_read_interface(     (kal_uint8)(bq24196_CON8), 
                                    (&val),
                                    (kal_uint8)(CON8_VBUS_STAT_MASK),
                                    (kal_uint8)(CON8_VBUS_STAT_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);	
	
    return val;
}

kal_uint32 bq24196_get_chrg_stat(void)
{
    kal_uint32 ret=0;
    kal_uint8 val=0;

    ret=bq24196_read_interface(     (kal_uint8)(bq24196_CON8), 
                                    (&val),
                                    (kal_uint8)(CON8_CHRG_STAT_MASK),
                                    (kal_uint8)(CON8_CHRG_STAT_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);	
	
    return val;
}

kal_uint32 bq24196_get_vsys_stat(void)
{
    kal_uint32 ret=0;
    kal_uint8 val=0;

    ret=bq24196_read_interface(     (kal_uint8)(bq24196_CON8), 
                                    (&val),
                                    (kal_uint8)(CON8_VSYS_STAT_MASK),
                                    (kal_uint8)(CON8_VSYS_STAT_SHIFT)
                                    );
    if(g_bq24196_log_en>1)        
        dprintf(CRITICAL, "%d\n", ret);	
	
    return val;
}

/**********************************************************
  *
  *   [Internal Function] 
  *
  *********************************************************/
void bq24196_dump_register(void)
{
#if 0
    U8 chip_slave_address = bq24196_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;
    int i=0;
    for (i=0;i<bq24196_REG_NUM;i++)
    {
        cmd = i;
        bq24196_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
        bq24196_reg[i] = data;
        printf("[bq24196_dump_register] Reg[0x%X]=0x%X\n", i, bq24196_reg[i]);
    }	
#else
    int i=0;

    for (i=0;i<bq24196_REG_NUM;i++)
    {
        bq24196_read_byte(i, &bq24196_reg[i]);
        dprintf(INFO, "[0x%x]=0x%x\n", i, bq24196_reg[i]);		  
    }
#endif
}
void bq24196_hw_init(void)
{
    upmu_set_rg_bc11_bb_ctrl(1);    //BC11_BB_CTRL    
    upmu_set_rg_bc11_rst(1);        //BC11_RST

    //TODO: pull PSEL low
    //TODO: pull CE low
    bq24196_set_en_hiz(0x0);
    bq24196_set_vindpm(0xA); //VIN DPM check 4.68V
    bq24196_set_reg_rst(0x0);
	bq24196_set_wdt_rst(0x1); //Kick watchdog
//    if(upmu_get_cid() == 0x1020)
    if(0)
	    bq24196_set_sys_min(0x0); //Minimum system voltage 3.0V (MT6320 E1 workaround, disable powerpath)
    else	    
        bq24196_set_sys_min(0x5); //Minimum system voltage 3.5V		
	bq24196_set_iprechg(0x3); //Precharge current 512mA
	bq24196_set_iterm(0x0); //Termination current 128mA

#if defined(MTK_JEITA_STANDARD_SUPPORT)        
    if(g_temp_status == TEMP_NEG_10_TO_POS_0)
    {    
		bq24196_set_vreg(0x1F); //VREG 4.0V
    }
    else
    {
        if(g_temp_status == TEMP_POS_10_TO_POS_45)
		    bq24196_set_vreg(0x2C); //VREG 4.208V			
		else
		    bq24196_set_vreg(0x25); //VREG 4.096V
    }     
#else
    bq24196_set_vreg(0x2C); //VREG 4.208V
#endif    
    bq24196_set_batlowv(0x1); //BATLOWV 3.0V
    bq24196_set_vrechg(0x0); //VRECHG 0.1V (4.108V)
    bq24196_set_en_term(0x1); //Enable termination
    bq24196_set_term_stat(0x0); //Match ITERM
    bq24196_set_watchdog(0x1); //WDT 40s
    bq24196_set_en_timer(0x0); //Disable charge timer
    bq24196_set_int_mask(0x0); //Disable fault interrupt
}

#if 1
kal_uint32 charging_get_charger_type(void *data);
static CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;
static int temp_CC_value = 0;
#endif

void bq24196_charging_enable(kal_uint32 bEnable)
{
    //TBD: set input current limit depends on connected charger type.
    #if 1
    charging_get_charger_type(&CHR_Type_num);
    if ( CHR_Type_num == STANDARD_HOST )
    {
        temp_CC_value = 500;
        bq24196_set_iinlim(0x2); //IN current limit at 500mA
        //bq24196_set_ac_current();
        bq24196_set_ichg(0);  //Fast Charging Current Limit at 500mA
    }
    else if (CHR_Type_num == NONSTANDARD_CHARGER)
    {
        temp_CC_value = 500;
        bq24196_set_iinlim(0x2); //IN current limit at 500mA
        //bq24196_set_ac_current();
        bq24196_set_ichg(0);  //Fast Charging Current Limit at 500mA
    }
    else if (CHR_Type_num == STANDARD_CHARGER)
    {
        temp_CC_value = 2000;
        bq24196_set_iinlim(0x6); //IN current limit at 2A
        //bq24196_set_ac_current();
        //(2000 - 500)/64 = 0x17;
        bq24196_set_ichg(0x17);  //Fast Charging Current Limit at 2A
    }
    else if (CHR_Type_num == CHARGING_HOST)
    {
        temp_CC_value = 500;
        bq24196_set_iinlim(0x2); //IN current limit at 500mA
        //bq24196_set_ac_current();
        bq24196_set_ichg(0);  //Fast Charging Current Limit at 500mA
    }
    else
    {
        dprintf(INFO, "[BATTERY:bq24196] Unknown charger type\n");
        temp_CC_value = 500;
        bq24196_set_iinlim(0x2); //IN current limit at 500mA
        //bq24196_set_ac_current();
        bq24196_set_ichg(0);  //Fast Charging Current Limit at 500mA
        //bq24196_set_low_chg_current();
    }
    #endif

    bq24196_set_en_hiz(0x0);	        	

    if(KAL_TRUE == bEnable)
        bq24196_set_chg_config(0x1); // charger enable
    else
        bq24196_set_chg_config(0x0); // charger disable

    dprintf(INFO, "[BATTERY:bq24196] bq24196_set_ac_current(), CC value(%dmA) \r\n", temp_CC_value);
    dprintf(INFO, "[BATTERY:bq24196] charger enable !\r\n");
}

#if defined(CONFIG_POWER_EXT)
#else

kal_uint32 Enable_Detection_Log = 0;
extern void Charger_Detect_Init(void);
extern void Charger_Detect_Release(void);
extern void mdelay (unsigned long msec);

static void hw_bc11_dump_register(void)
{
	kal_uint32 reg_val = 0;
	kal_uint32 reg_num = CHR_CON18;
	kal_uint32 i = 0;

	for(i=reg_num ; i<=CHR_CON19 ; i+=2)
	{
		reg_val = upmu_get_reg_value(i);
		dprintf(INFO, "Chr Reg[0x%x]=0x%x \r\n", i, reg_val);
	}	
}

static void hw_bc11_init(void)
{
    mdelay(300);
    Charger_Detect_Init();

    //RG_BC11_BIAS_EN=1	
    upmu_set_rg_bc11_bias_en(0x1);
    //RG_BC11_VSRC_EN[1:0]=00
    upmu_set_rg_bc11_vsrc_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_IPD_EN[1.0] = 00
    upmu_set_rg_bc11_ipd_en(0x0);
    //BC11_RST=1
    upmu_set_rg_bc11_rst(0x1);
    //BC11_BB_CTRL=1
    upmu_set_rg_bc11_bb_ctrl(0x1);

    //msleep(10);
    mdelay(50);

    if(Enable_Detection_Log)
    {
        dprintf(INFO, "hw_bc11_init() \r\n");
        hw_bc11_dump_register();
    }	
}
 
static U32 hw_bc11_DCD(void)
{
    U32 wChargerAvail = 0;

    //RG_BC11_IPU_EN[1.0] = 10
    upmu_set_rg_bc11_ipu_en(0x2);
    //RG_BC11_IPD_EN[1.0] = 01
    upmu_set_rg_bc11_ipd_en(0x1);
    //RG_BC11_VREF_VTH = [1:0]=01
    upmu_set_rg_bc11_vref_vth(0x1);
    //RG_BC11_CMP_EN[1.0] = 10
    upmu_set_rg_bc11_cmp_en(0x2);

    //msleep(20);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
        dprintf(INFO, "hw_bc11_DCD() \r\n");
        hw_bc11_dump_register();
    }

    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_IPD_EN[1.0] = 00
    upmu_set_rg_bc11_ipd_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);

    return wChargerAvail;
}

static U32 hw_bc11_stepA1(void)
{
    U32 wChargerAvail = 0;
      
    //RG_BC11_IPU_EN[1.0] = 10
    upmu_set_rg_bc11_ipu_en(0x2);
    //RG_BC11_VREF_VTH = [1:0]=10
    upmu_set_rg_bc11_vref_vth(0x2);
    //RG_BC11_CMP_EN[1.0] = 10
    upmu_set_rg_bc11_cmp_en(0x2);

    //msleep(80);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_stepA1() \r\n");
    	hw_bc11_dump_register();
    }

    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);

    return  wChargerAvail;
}


static U32 hw_bc11_stepB1(void)
{
    U32 wChargerAvail = 0;
      
    //RG_BC11_IPU_EN[1.0] = 01
    //upmu_set_rg_bc11_ipu_en(0x1);
    upmu_set_rg_bc11_ipd_en(0x1);
    //RG_BC11_VREF_VTH = [1:0]=10
    //upmu_set_rg_bc11_vref_vth(0x2);
    upmu_set_rg_bc11_vref_vth(0x0);
    //RG_BC11_CMP_EN[1.0] = 01
    upmu_set_rg_bc11_cmp_en(0x1);

    //msleep(80);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_stepB1() \r\n");
    	hw_bc11_dump_register();
    }

    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);

    return  wChargerAvail;
}


static U32 hw_bc11_stepC1(void)
{
    U32 wChargerAvail = 0;
      
    //RG_BC11_IPU_EN[1.0] = 01
    upmu_set_rg_bc11_ipu_en(0x1);
    //RG_BC11_VREF_VTH = [1:0]=10
    upmu_set_rg_bc11_vref_vth(0x2);
    //RG_BC11_CMP_EN[1.0] = 01
    upmu_set_rg_bc11_cmp_en(0x1);

    //msleep(80);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_stepC1() \r\n");
    	hw_bc11_dump_register();
    }

    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);

    return  wChargerAvail;
}


static U32 hw_bc11_stepA2(void)
{
    U32 wChargerAvail = 0;
      
    //RG_BC11_VSRC_EN[1.0] = 10 
    upmu_set_rg_bc11_vsrc_en(0x2);
    //RG_BC11_IPD_EN[1:0] = 01
    upmu_set_rg_bc11_ipd_en(0x1);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);
    //RG_BC11_CMP_EN[1.0] = 01
    upmu_set_rg_bc11_cmp_en(0x1);

    //msleep(80);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_stepA2() \r\n");
    	hw_bc11_dump_register();
    }

    //RG_BC11_VSRC_EN[1:0]=00
    upmu_set_rg_bc11_vsrc_en(0x0);
    //RG_BC11_IPD_EN[1.0] = 00
    upmu_set_rg_bc11_ipd_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);

    return  wChargerAvail;
}


static U32 hw_bc11_stepB2(void)
{
    U32 wChargerAvail = 0;

    //RG_BC11_IPU_EN[1:0]=10
    upmu_set_rg_bc11_ipu_en(0x2);
    //RG_BC11_VREF_VTH = [1:0]=10
    upmu_set_rg_bc11_vref_vth(0x1);
    //RG_BC11_CMP_EN[1.0] = 01
    upmu_set_rg_bc11_cmp_en(0x1);

    //msleep(80);
    mdelay(80);

    wChargerAvail = upmu_get_rgs_bc11_cmp_out();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_stepB2() \r\n");
    	hw_bc11_dump_register();
    }

    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=00
    upmu_set_rg_bc11_vref_vth(0x0);

    return  wChargerAvail;
}


static void hw_bc11_done(void)
{
    //RG_BC11_VSRC_EN[1:0]=00
    upmu_set_rg_bc11_vsrc_en(0x0);
    //RG_BC11_VREF_VTH = [1:0]=0
    upmu_set_rg_bc11_vref_vth(0x0);
    //RG_BC11_CMP_EN[1.0] = 00
    upmu_set_rg_bc11_cmp_en(0x0);
    //RG_BC11_IPU_EN[1.0] = 00
    upmu_set_rg_bc11_ipu_en(0x0);
    //RG_BC11_IPD_EN[1.0] = 00
    upmu_set_rg_bc11_ipd_en(0x0);
    //RG_BC11_BIAS_EN=0
    upmu_set_rg_bc11_bias_en(0x0); 

    Charger_Detect_Release();

    if(Enable_Detection_Log)
    {
    	dprintf(INFO, "hw_bc11_done() \r\n");
    	hw_bc11_dump_register();
    }

}
#endif

kal_uint32 charging_get_charger_type(void *data)
 {
	 kal_uint32 status = 0;
#if defined(CONFIG_POWER_EXT)
	 *(CHARGER_TYPE*)(data) = STANDARD_HOST;
#else
    
#if 0
    CHARGER_TYPE charger_type = CHARGER_UNKNOWN;
    charger_type = hw_charger_type_detection();
    battery_xlog_printk(BAT_LOG_CRTI, "charging_get_charger_type = %d\r\n", charger_type);
        
    *(CHARGER_TYPE*)(data) = charger_type;
#endif

#if 1
	/********* Step initial  ***************/		 
	hw_bc11_init();
 
	/********* Step DCD ***************/  
	if(1 == hw_bc11_DCD())
	{
		 /********* Step A1 ***************/
		 if(1 == hw_bc11_stepA1())
		 {
			 /********* Step B1 ***************/
			 if(1 == hw_bc11_stepB1())
			 {
				 //*(CHARGER_TYPE*)(data) = NONSTANDARD_CHARGER;
				 //battery_xlog_printk(BAT_LOG_CRTI, "step B1 : Non STANDARD CHARGER!\r\n");				
				 *(CHARGER_TYPE*)(data) = APPLE_2_1A_CHARGER;
				 dprintf(INFO, "step B1 : Apple 2.1A CHARGER!\r\n");
			 }	 
			 else
			 {
				 //*(CHARGER_TYPE*)(data) = APPLE_2_1A_CHARGER;
				 //battery_xlog_printk(BAT_LOG_CRTI, "step B1 : Apple 2.1A CHARGER!\r\n");
				 *(CHARGER_TYPE*)(data) = NONSTANDARD_CHARGER;
				 dprintf(INFO, "step B1 : Non STANDARD CHARGER!\r\n");
			 }	 
		 }
		 else
		 {
			 /********* Step C1 ***************/
			 if(1 == hw_bc11_stepC1())
			 {
				 *(CHARGER_TYPE*)(data) = APPLE_1_0A_CHARGER;
				 dprintf(INFO, "step C1 : Apple 1A CHARGER!\r\n");
			 }	 
			 else
			 {
				 *(CHARGER_TYPE*)(data) = APPLE_0_5A_CHARGER;
				 dprintf(INFO, "step C1 : Apple 0.5A CHARGER!\r\n");			 
			 }	 
		 }
 
	}
	else
	{
		 /********* Step A2 ***************/
		 if(1 == hw_bc11_stepA2())
		 {
			 /********* Step B2 ***************/
			 if(1 == hw_bc11_stepB2())
			 {
				 *(CHARGER_TYPE*)(data) = STANDARD_CHARGER;
				 dprintf(INFO, "step B2 : STANDARD CHARGER!\r\n");
			 }
			 else
			 {
				 *(CHARGER_TYPE*)(data) = CHARGING_HOST;
				 dprintf(INFO, "step B2 :  Charging Host!\r\n");
			 }
		 }
		 else
		 {
         *(CHARGER_TYPE*)(data) = STANDARD_HOST;
			 dprintf(INFO, "step A2 : Standard USB Host!\r\n");
		 }
 
	}
 
	 /********* Finally setting *******************************/
	 hw_bc11_done();
#endif
#endif
	 return status;
}
