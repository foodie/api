/**
 * @(#)CommEvent.java	6.3.9 09/10/02
 *
 * Copyright 2000-2010 MyMMSC Software Foundation (MSF), Inc. All rights reserved.
 * MyMMSC PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
package org.mymmsc.w3c.gsm;

import javax.comm.SerialPortEvent;

/**
 * ����ͨѶ�¼�
 * 
 * @author WangFeng(wangfeng@yeah.net)
 * @version 6.3.9 09/10/02
 * @since mymmsc-gsm 6.3.9
 */
public final class CommEvent {

	private static String BI = "ͨѶ�ж�";
	private static String CD = "�ز����";
	private static String CTS = "�������";
	private static String DATA_AVAILABLE = "�����ݵ���";
	private static String DSR = "�����豸׼����";
	private static String FE = "֡����";
	private static String OE = "��λ����";
	private static String OUTPUT_BUFFER_EMPTY = "��������������";
	private static String PE = "��żУ���";
	private static String RI = "����ָʾ";

	public static String getDesc(int EventType) {
		String tmpDesc = "";
		switch (EventType) {
		case SerialPortEvent.CTS:
			System.out.println("CTS event occured.");
			tmpDesc = CTS;
			break;
		case SerialPortEvent.CD:
			System.out.println("CD event occured.");
			tmpDesc = CD;
			break;
		case SerialPortEvent.BI:
			System.out.println("BI event occured.");
			tmpDesc = BI;
			break;
		case SerialPortEvent.DSR:
			System.out.println("DSR event occured.");
			tmpDesc = DSR;
			break;
		case SerialPortEvent.FE:
			System.out.println("FE event occured.");
			tmpDesc = FE;
			break;
		case SerialPortEvent.OE:
			System.out.println("OE event occured.");
			tmpDesc = OE;
			break;
		case SerialPortEvent.PE:
			System.out.println("PE event occured.");
			tmpDesc = PE;
			break;
		case SerialPortEvent.RI:
			System.out.println("RI event occured.");
			tmpDesc = RI;
			break;
		case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
			System.out.println("OUTPUT_BUFFER_EMPTY event occured.");
			tmpDesc = OUTPUT_BUFFER_EMPTY;
			break;
		case SerialPortEvent.DATA_AVAILABLE:
			System.out.println("DATA_AVAILABLE event occured.");
			tmpDesc = DATA_AVAILABLE;
			break;
		}
		return tmpDesc;

	}
}
