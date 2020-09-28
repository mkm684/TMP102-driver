// ISR(TWI_vect)
// {
//     switch(TWSR & 0xF8)
//     {    
//         case START:
//             UART_putStr("START\n");
//             if (type == READ)
//                 TWDR = (addr << 1) | 0x01;    
//             else
//                 TWDR = (addr << 1) & 0xFE;
//             break;             
//         case REPEATED_START:
//             UART_putStr("REPEATED_START\n");
//             if (type == READ)
//                 TWDR = (addr << 1) | 0x01;    
//             else
//                 TWDR = (addr << 1) & 0xFE;
//             break;
//         case MT_SLA_ACK:
//             UART_putStr("MT_SLA_ACK\n");
//             if(i < size) {
//                 TWDR = data[i];
//                 i++;
//             }
//             break;        
//         case MT_SLA_NOACK || MT_SLA_DATA_NOACK:
//             UART_putStr("MT_SLA_NOACK or MT_SLA_DATA_NOACK\n");
//             break;      
//         case MT_SLA_DATA_ACK:
//             UART_putStr("MT_SLA_DATA_ACK\n");
//             if(i < size) {
//                 UART_putStr("HERE\n");
//                 UART_puthex8(i);
//                 TWDR = data[i];
//                 i++;
//             } else {
//                 UART_putStr("STOP\n");
//                 TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);    
//                 i = 0;
//                 done = 1;
//             }
//             break;    
//         case MT_SLA_ACK_R:
//             UART_putStr("MT_SLA_ACK_R\n");
//             if (bytes_num < 2) {   
//                 byte[bytes_num] = (uint8_t)TWDR;
//                 bytes_num++;
//             }
//             break;        
//         case MT_SLA_NOACK_R:
//             UART_putStr("MT_SLA_NOACK_R\n");
//             break;      
//         case MT_SLA_DATA_ACK_R:
//             UART_putStr("MT_SLA_DATA_ACK_R\n");
//             if (bytes_num < 2) {   
//                 byte[bytes_num] = (uint8_t)TWDR;
//                 _i2cStatus(MT_SLA_DATA_ACK_R, MT_SLA_DATA_NOACK_R);
//                 bytes_num++;
//             } else {
//                 UART_putStr("STOP\n");
//                 TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
//                 bytes_num = 0;
//                 done = 1;

//                 temp = ((int16_t)byte[0] << 4) | (byte[1] >> 4);
//                 if (temp > 0x7FF) 
//                     temp |= 0xF000;
//             }
//             break;   
//         case MT_SLA_DATA_ACK_REP_R:
//             UART_putStr("MT_SLA_DATA_ACK_REP_R\n");
//             break;  
//         case MT_SLA_DATA_NOACK_R:
//             UART_putStr("MT_SLA_DATA_NOACK_R\n");
//             break;
//     }
//     TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);    
//     // cli();
// }
